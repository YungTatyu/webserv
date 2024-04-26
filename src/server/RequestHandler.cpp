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

    // timeout追加
    // TODO: 本来client_header_timeoutだが、client_request_timeoutというのを後で作る。
    this->addTimerByType(ioHandler, connManager, configHandler, timerTree, new_sock, Timer::TMO_CLI_REQUEST);

    // worker_connections確認
    if (this->isOverWorkerConnections(connManager, configHandler))
      this->deleteTimerAndConnection(ioHandler, connManager, timerTree,
                                     timerTree.getTimerTree().begin()->getFd());

    return new_sock;
  }

  // keepalive_timeout消す。
  timerTree.deleteTimer(sockfd);

  // クライアントソケットへのリクエスト（既存コネクション）
  ssize_t re = ioHandler.receiveRequest(connManager, sockfd);
  if (re == -1)  //ソケット使用不可。
    return RequestHandler::UPDATE_NONE;
  if (re == 0)  // クライアントが接続を閉じる
  {
    ioHandler.closeConnection(connManager, sockfd);
    return RequestHandler::UPDATE_CLOSE;
  }
  if (re == 2)  // buffer分以降を読む
    return RequestHandler::UPDATE_NONE;
  const std::vector<unsigned char> &context = connManager.getRawRequest(sockfd);
  // reinterpret_cast<const char*>を使うと、文字の長さにバグが生じる
  std::string requestData = std::string(context.begin(), context.end());

  HttpRequest::parseRequest(requestData, connManager.getRequest(sockfd));

  HttpRequest::ParseState state = connManager.getRequest(sockfd).parseState;
  if (state != HttpRequest::PARSE_COMPLETE &&
      state !=
          HttpRequest::
              PARSE_ERROR)  // 新しいHttpRequestを使う時にここを有効にしてchunk読み中はreadイベントのままにする
    return RequestHandler::UPDATE_NONE;
  return handleResponse(ioHandler, connManager, configHandler, timerTree, sockfd);
}

int RequestHandler::handleResponse(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                   ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);
  std::string final_response = HttpResponse::generateResponse(
      request, response, connManager.getTiedServer(sockfd), sockfd, configHandler);

  if (response.state_ == HttpResponse::RES_EXECUTE_CGI)
    return handleCgi(ioHandler, connManager, configHandler, timerTree, sockfd);
  connManager.setFinalResponse(sockfd,
                               std::vector<unsigned char>(final_response.begin(), final_response.end()));

  connManager.setEvent(sockfd, ConnectionData::EV_WRITE);  // writeイベントに更新
  // sendtimeout 追加
  this->addTimerByType(ioHandler, connManager, configHandler, timerTree, sockfd, Timer::TMO_KEEPALIVE);
  return RequestHandler::UPDATE_WRITE;
}

int RequestHandler::handleCgi(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                              ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);

  bool re = connManager.callCgiExecutor(sockfd, response.res_file_path_, request);
  if (!re) {
    connManager.resetCgiSockets(sockfd);
    response.state_ = HttpResponse::RES_CGI_ERROR;
    return handleResponse(ioHandler, connManager, configHandler, timerTree, sockfd);
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
  int re = ioHandler.receiveCgiResponse(connManager, sockfd);
  if (re == -1 || re == 2)  // recv errorまたはbuffer size分recv
    return RequestHandler::UPDATE_NONE;
  const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(sockfd);
  if (!cgiProcessExited(cgi_handler.getCgiProcessId())) return RequestHandler::UPDATE_NONE;

  const std::vector<unsigned char> &v = connManager.getCgiResponse(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);
  std::string res(v.begin(), v.end());
  bool parse_suc = connManager.callCgiParser(sockfd, response, res);
  response.state_ = parse_suc ? HttpResponse::RES_PARSED_CGI : HttpResponse::RES_CGI_ERROR;
  re = handleResponse(ioHandler, connManager, configHandler, timerTree, sockfd);
  ioHandler.closeConnection(connManager, sockfd);  // delete cgi event
  return re;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                     ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
    return handleCgiWriteEvent(ioHandler, connManager, sockfd);
  int re = ioHandler.sendResponse(connManager, sockfd);

  // send_timeout削除
  timerTree.deleteTimer(sockfd);

  /* -1: send error, retry later
   * -2: send not complete, send remainder later
   */
  if (re != 0) {
    // send_timeout追加
    this->addTimerByType(ioHandler, connManager, configHandler, timerTree, sockfd, Timer::TMO_SEND);
    return RequestHandler::UPDATE_NONE;
  }

  if (!this->addTimerByType(ioHandler, connManager, configHandler, timerTree, sockfd, Timer::TMO_KEEPALIVE))
    return UPDATE_CLOSE;

  // readイベントに更新
  connManager.setEvent(sockfd, ConnectionData::EV_READ);
  // connection dataを削除
  connManager.clearConnectionData(sockfd);
  return RequestHandler::UPDATE_READ;
}

int RequestHandler::handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        const int sockfd) {
  ssize_t re = ioHandler.sendRequestBody(connManager, sockfd);

  const std::string body = connManager.getRequest(sockfd).body;
  const cgi::CGIHandler cgi_handler = connManager.getCgiHandler(sockfd);
  if (connManager.getSentBytes(sockfd) == body.size() ||              // bodyを全て送ったら
      (re == -1 && cgiProcessExited(cgi_handler.getCgiProcessId())))  // cgi processがすでに死んでいたら
  {
    connManager.resetSentBytes(sockfd);
    connManager.setEvent(sockfd, ConnectionData::EV_CGI_READ);
    return RequestHandler::UPDATE_CGI_READ;
  }
  return RequestHandler::UPDATE_NONE;
}

int RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                     TimerTree &timerTree, const int sockfd) {
  ioHandler.closeConnection(connManager, sockfd);
  timerTree.deleteTimer(sockfd);
  return RequestHandler::UPDATE_CLOSE;
}

void RequestHandler::handleTimeoutEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        ConfigHandler &configHandler, TimerTree &timerTree) {
  configHandler.writeErrorLog("webserv: [debug] enter timeout handler\n");
  // timeoutしていない最初のイテレータを取得
  Timer current_time(-1, 0);
  std::multiset<Timer>::iterator upper_bound = timerTree.getTimerTree().upper_bound(current_time);

  Timer::updateCurrentTime();
  // timeout している接続をすべて削除
  for (std::multiset<Timer>::iterator it = timerTree.getTimerTree().begin(); it != upper_bound;) {
    std::multiset<Timer>::iterator next = it;
    next++;
    // timer tree から削除
    this->deleteTimerAndConnection(ioHandler, connManager, timerTree, it->getFd());
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
  // TODO: errorのときの処理はあやしい
  if (re == 0 || re == -1) return false;
  return true;
}

/**
 * @brief TimerTypeと直前のresponseのヘッダーに従ってtimeout値を取得し、TimerTreeにtimerを追加する。
 *
 * @param NetworkIOHandler, ConnectionManager, ConfigHandler, TimerTree, socket, TimeoutType
 * @return true: timerを追加
 * @return false: 'Connections: closeの'場合
 */
bool RequestHandler::addTimerByType(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                    ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd,
                                    enum Timer::TimeoutType type) {
  config::Time timeout;
  std::map<std::string, std::string>::iterator it;

  // Connectionヘッダーを見てcloseならコネクションを閉じる
  if (type == Timer::TMO_KEEPALIVE) {
    it = connManager.getResponse(sockfd).headers_.find("Connection");
    if (it != connManager.getResponse(sockfd).headers_.end() && it->second == "close") {
      ioHandler.closeConnection(connManager, sockfd);
      return false;
    }
  }

  // Hostヘッダーがあるか確認
  // 400エラーがerror_pageで拾われて内部リダイレクトする可能性があるので以下の処理は必要。
  // このように探すdirectiveがほんとにこのクライアントが最後にアクセスしたコンテキストかは怪しい。
  // TODO: 毎回設定値見に行くの計算量かかる？
  it = connManager.getRequest(sockfd).headers.find("Host");
  std::string host_name;
  if (it == connManager.getRequest(sockfd).headers.end())
    host_name = "_";
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

  // 設定値が0ならばタイムアウトを設定しない
  if (timeout.isNoTime()) {
    return true;
  }

  timerTree.addTimer(Timer(sockfd, timeout));

  return true;
}

void RequestHandler::deleteTimerAndConnection(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                              TimerTree &timerTree, int socket) {
  // もしCGIソケットなら紐づくclientソケットも削除
  if (connManager.isCgiSocket(socket)) {
    connManager.getCgiHandler(socket).killCgiProcess();
    int tied_sock = connManager.getCgiHandler(socket).getCliSocket();
    ioHandler.closeConnection(connManager, tied_sock);
    timerTree.deleteTimer(tied_sock);
  }
  timerTree.deleteTimer(socket);
  ioHandler.closeConnection(connManager, socket);
}

bool RequestHandler::isOverWorkerConnections(ConnectionManager &connManager, ConfigHandler &configHandler) {
  return (connManager.getConnections().size() - connManager.getCgiSockNum()) >=
         configHandler.config_->events.worker_connections.getWorkerConnections();
}
