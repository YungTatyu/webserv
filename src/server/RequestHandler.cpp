#include "RequestHandler.hpp"

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <algorithm>

#include "HttpResponse.hpp"

RequestHandler::RequestHandler() {}

int RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                    ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_READ)
    return handleCgiReadEvent(ioHandler, connManager, configHandler, timerTree, sockfd);
  // リスニングソケットへの新規リクエスト
  if (ioHandler.isListenSocket(sockfd)) {
    int new_sock = ioHandler.acceptConnection(connManager, sockfd);
    if (new_sock == -1) return new_sock;

    // TODO: 本来client_header_timeoutだが、client_request_timeoutというのを後で作る。
    this->addTimerByType(connManager, configHandler, timerTree, new_sock, Timer::TMO_CLI_REQUEST);
    if (!this->isOverWorkerConnections(connManager, configHandler)) return new_sock;
    int timeout_fd = timerTree.getClosestTimeout();
    // TODO: cgiの時はどうする? nginxの場合は、新しいクライアントのリクエストを受け付けない
    if (connManager.isCgiSocket(timeout_fd)) {
      const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(timeout_fd);
      int client = cgi_handler.getCliSocket();
      ioHandler.closeConnection(connManager, timerTree, timeout_fd);  // cgiから削除する
      ioHandler.closeConnection(connManager, timerTree, client);
      return new_sock;
    }
    ioHandler.closeConnection(connManager, timerTree, timeout_fd);
    return new_sock;
  }

  // keepalive_timeout消す。
  timerTree.deleteTimer(sockfd);

  // クライアントソケットへのリクエスト（既存コネクション）
  ssize_t re = ioHandler.receiveRequest(connManager, sockfd);
  // ソケット使用不可。
  if (re == -1) return RequestHandler::UPDATE_NONE;
  if (re == 0)  // クライアントが接続を閉じる
  {
    ioHandler.closeConnection(connManager, timerTree, sockfd);
    return RequestHandler::UPDATE_CLOSE;
  }
  const std::vector<unsigned char> &context = connManager.getRawRequest(sockfd);
  // reinterpret_cast<const char*>を使うと、文字の長さにバグが生じる
  std::string request = std::string(context.begin(), context.end());
  return handleRequest(connManager, configHandler, timerTree, sockfd, request);
}

int RequestHandler::handleRequest(ConnectionManager &connManager, ConfigHandler &configHandler,
                                  TimerTree &timerTree, const int sockfd, std::string &rawRequest) {
  HttpRequest::parseRequest(rawRequest, connManager.getRequest(sockfd));

  HttpRequest::ParseState state = connManager.getRequest(sockfd).parseState;
  // 新しいHttpRequestを使う時にここを有効にしてchunk読み中はreadイベントのままにする
  if (state != HttpRequest::PARSE_COMPLETE && state != HttpRequest::PARSE_ERROR) {
    connManager.clearRawRequest(sockfd);
    return RequestHandler::UPDATE_NONE;
  }
  return handleResponse(connManager, configHandler, timerTree, sockfd, rawRequest);
}

int RequestHandler::handleResponse(ConnectionManager &connManager, ConfigHandler &configHandler,
                                   TimerTree &timerTree, const int sockfd, std::string &rawRequest) {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);
  std::string final_response = HttpResponse::generateResponse(
      request, response, connManager.getTiedServer(sockfd), sockfd, configHandler);

  if (response.state_ == HttpResponse::RES_EXECUTE_CGI)
    return handleCgi(connManager, configHandler, timerTree, sockfd);
  connManager.addFinalResponse(sockfd,
                               std::vector<unsigned char>(final_response.begin(), final_response.end()));

  HttpRequest::ParseState state = connManager.getRequest(sockfd).parseState;
  // 複数のリクエストが来た場合、全てparseする
  // parseした結果がエラーの場合は、それ以上のリクエストはみない
  if (state == HttpRequest::PARSE_COMPLETE && !rawRequest.empty())
    return handleRequest(connManager, configHandler, timerTree, sockfd, rawRequest);

  // send開始するまでのtimout追加
  // cgiだったら紐づくclient_socketにタイマーを設定する
  int client = connManager.isCgiSocket(sockfd) ? connManager.getCgiHandler(sockfd).getCliSocket() : sockfd;
  this->addTimerByType(connManager, configHandler, timerTree, client, Timer::TMO_KEEPALIVE);

  connManager.setEvent(sockfd, ConnectionData::EV_WRITE);  // writeイベントに更新
  return RequestHandler::UPDATE_WRITE;
}

int RequestHandler::handleCgi(ConnectionManager &connManager, ConfigHandler &configHandler,
                              TimerTree &timerTree, const int sockfd) {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);

  bool re = connManager.callCgiExecutor(sockfd, response.res_file_path_, request);
  if (!re) {
    connManager.resetCgiSockets(sockfd);
    response.state_ = HttpResponse::RES_CGI_ERROR;
    std::string next_req;
    return handleResponse(connManager, configHandler, timerTree, sockfd, next_req);
  }
  // bodyが空なら、bodyをsendしない
  if (request.body.empty()) {
    connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_READ);
    return RequestHandler::UPDATE_CGI_READ;
  }
  connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_WRITE);
  return RequestHandler::UPDATE_CGI_WRITE;
}

int RequestHandler::handleCgiReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                       ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  ssize_t re = ioHandler.receiveCgiResponse(connManager, sockfd);
  // recv error
  if (re == -1) return RequestHandler::UPDATE_NONE;
  const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(sockfd);
  if (!cgiProcessExited(cgi_handler.getCgiProcessId())) return RequestHandler::UPDATE_NONE;

  const std::vector<unsigned char> &v = connManager.getCgiResponse(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);
  std::string res(v.begin(), v.end());
  bool parse_suc = connManager.callCgiParser(sockfd, response, res);
  response.state_ = parse_suc ? HttpResponse::RES_PARSED_CGI : HttpResponse::RES_CGI_ERROR;
  std::string next_req;
  re = handleResponse(connManager, configHandler, timerTree, sockfd, next_req);
  ioHandler.closeConnection(connManager, timerTree, sockfd);  // delete cgi event
  return re;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                     ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
    return handleCgiWriteEvent(ioHandler, connManager, sockfd);

  ssize_t re = ioHandler.sendResponse(connManager, sockfd);

  ssize_t buff_size = static_cast<ssize_t>(ioHandler.getBufferSize());
  // error
  if (re == -1) return RequestHandler::UPDATE_NONE;
  // client connection closed
  if (re == 0) {
    ioHandler.closeConnection(connManager, timerTree, sockfd);
    return RequestHandler::UPDATE_CLOSE;
  }
  // 引き続きresponseを送信
  if (re == buff_size) {
    this->addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_SEND);
    return RequestHandler::UPDATE_NONE;
  }

  if (!this->addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_KEEPALIVE)) {
    // Connection: closeなので接続閉じる
    ioHandler.closeConnection(connManager, timerTree, sockfd);
    return UPDATE_CLOSE;
  }

  // readイベントに更新
  connManager.setEvent(sockfd, ConnectionData::EV_READ);
  // connection dataを削除
  connManager.clearConnectionData(sockfd);
  return RequestHandler::UPDATE_READ;
}

int RequestHandler::handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        const int sockfd) {
  ioHandler.sendRequestBody(connManager, sockfd);

  const std::string body = connManager.getRequest(sockfd).body;
  const cgi::CGIHandler cgi_handler = connManager.getCgiHandler(sockfd);
  if (connManager.getSentBytes(sockfd) == body.size() ||  // bodyを全て送ったら
      (cgiProcessExited(cgi_handler.getCgiProcessId())))  // cgi processがすでに死んでいたら
  {
    connManager.resetSentBytes(sockfd);
    connManager.setEvent(sockfd, ConnectionData::EV_CGI_READ);
    return RequestHandler::UPDATE_CGI_READ;
  }
  return RequestHandler::UPDATE_NONE;
}

int RequestHandler::handleEofEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                   ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_READ) {
    return handleCgiReadEvent(ioHandler, connManager, configHandler, timerTree, sockfd);
  } else if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE) {
    return RequestHandler::UPDATE_CGI_READ;
  }
  ioHandler.closeConnection(connManager, timerTree, sockfd);
  return RequestHandler::UPDATE_CLOSE;
}

int RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                     TimerTree &timerTree, const int sockfd) {
  ioHandler.closeConnection(connManager, timerTree, sockfd);
  return RequestHandler::UPDATE_CLOSE;
}

void RequestHandler::handleTimeoutEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        ConfigHandler &configHandler, TimerTree &timerTree) {
  configHandler.writeErrorLog("webserv: [debug] enter timeout handler\n");
  // timeoutしていない最初のイテレータを取得
  Timer current_time(-1, 0);
  std::multiset<Timer>::iterator upper_bound = timerTree.getTimerTree().upper_bound(current_time);

  // timeout している接続をすべて削除
  for (std::multiset<Timer>::iterator it = timerTree.getTimerTree().begin(); it != upper_bound;) {
    std::multiset<Timer>::iterator next = it;
    next++;
    // timer tree から削除
    ioHandler.closeConnection(connManager, timerTree, it->getFd());
    // timeoutの種類によってログ出力変える
    // std::string	timeout_reason = "waiting for client request.";
    configHandler.writeErrorLog("webserv: [info] client timed out\n");
    it = next;
  }
}

/**
 * @brief cgi processが生きているか確認
 *
 * @param process_id
 * @return true cgi processが終了している
 * @return false
 */
bool RequestHandler::cgiProcessExited(const pid_t process_id) const {
  int status;
  pid_t re = waitpid(process_id, &status, WNOHANG);
  // errorまたはprocessが終了していない
  // errorのときの処理はあやしい, -1のエラーはロジック的にありえない(process idがおかしい)
  if (re == 0 || re == -1) return false;
  return true;
}

/**
 * @brief TimerTypeと直前のresponseのヘッダーに従ってtimeout値を取得し、TimerTreeにtimerを追加する。
 *
 * @param NetworkIOHandler, ConnectionManager, ConfigHandler, TimerTree, socket, TimeoutType
 *
 * @return true: N秒timerを追加
 * @return false: 'Connections: close'の場合
 */
bool RequestHandler::addTimerByType(ConnectionManager &connManager, ConfigHandler &configHandler,
                                    TimerTree &timerTree, const int sockfd, enum Timer::TimeoutType type) {
  config::Time timeout;
  std::map<std::string, std::string>::iterator it;

  // send後にConnectionヘッダーを見てcloseならコネクションを閉じる
  if (type == Timer::TMO_KEEPALIVE &&
      connManager.getConnections().at(sockfd)->event == ConnectionData::EV_WRITE) {
    it = connManager.getResponse(sockfd).headers_.find("Connection");
    if (it != connManager.getResponse(sockfd).headers_.end() && it->second == "close") {
      return false;
    }
  }

  // Hostヘッダーがあるか確認
  // 400エラーがerror_pageで拾われて内部リダイレクトする可能性があるので以下の処理は必要。
  // このように探すdirectiveがほんとにこのクライアントが最後にアクセスしたコンテキストかは怪しい。
  it = connManager.getRequest(sockfd).headers.find("Host");
  std::string host_name;
  if (it == connManager.getRequest(sockfd).headers.end())
    host_name = "";
  else
    host_name = it->second;

  // timeout時間セット
  switch (type) {
    case Timer::TMO_KEEPALIVE:
      timeout = configHandler.searchKeepaliveTimeout(connManager.getTiedServer(sockfd), host_name,
                                                     connManager.getRequest(sockfd).uri);
      break;

    case Timer::TMO_CLI_REQUEST:
      // TODO: ディレクティブ作る
      timeout = config::Time(60 * config::Time::seconds);
      break;

    case Timer::TMO_SEND:
      timeout = configHandler.searchSendTimeout(connManager.getTiedServer(sockfd), host_name,
                                                connManager.getRequest(sockfd).uri);
      break;
  }

  // 設定値が0ならばタイムアウトを設定しないで削除
  if (timeout.isNoTime()) {
    timerTree.deleteTimer(sockfd);
    return true;
  }

  timerTree.addTimer(Timer(sockfd, timeout));

  return true;
}

bool RequestHandler::isOverWorkerConnections(ConnectionManager &connManager, ConfigHandler &configHandler) {
  return (connManager.getConnections().size() - connManager.getCgiSockNum()) >=
         configHandler.config_->events.worker_connections.getWorkerConnections();
}
