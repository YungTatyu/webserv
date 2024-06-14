#include "RequestHandler.hpp"

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "HttpRequest.hpp"
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

    addTimerByType(connManager, configHandler, timerTree, new_sock, Timer::TMO_RECV);
    if (!isOverWorkerConnections(connManager, configHandler)) return new_sock;
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
  if (re == -1) return RequestHandler::UPDATE_READ;
  if (re == 0)  // クライアントが接続を閉じる
  {
    ioHandler.closeConnection(connManager, timerTree, sockfd);
    return RequestHandler::UPDATE_CLOSE;
  }
  return handleRequest(connManager, configHandler, timerTree, sockfd);
}

int RequestHandler::handleRequest(ConnectionManager &connManager, ConfigHandler &configHandler,
                                  TimerTree &timerTree, const int sockfd) {
  const std::vector<unsigned char> &context = connManager.getRawRequest(sockfd);
  // reinterpret_cast<const char*>を使うと、文字の長さにバグが生じる
  std::string raw_request = std::string(context.begin(), context.end());
  HttpRequest::parseRequest(raw_request, connManager.getRequest(sockfd));

  HttpRequest::ParseState state = connManager.getRequest(sockfd).parseState;
  // requestが未完の場合は引き続きrequestをまつ
  if (state != HttpRequest::PARSE_COMPLETE && state != HttpRequest::PARSE_ERROR &&
      state != HttpRequest::PARSE_NOT_IMPLEMENTED) {
    connManager.clearRawRequest(sockfd);
    return RequestHandler::UPDATE_READ;
  }
  // parseした分のraw requestを削除しなければいけない
  // 処理のオーバーヘッド？ 最適化の余地あり
  std::vector<unsigned char> v(raw_request.begin(), raw_request.end());
  connManager.setRawRequest(sockfd, v);
  return handleResponse(connManager, configHandler, timerTree, sockfd);
}

int RequestHandler::handleResponse(ConnectionManager &connManager, ConfigHandler &configHandler,
                                   TimerTree &timerTree, const int sockfd) {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);
  std::string final_response = HttpResponse::generateResponse(
      request, response, connManager.getTiedServer(sockfd), sockfd, configHandler);

  if (response.state_ == HttpResponse::RES_EXECUTE_CGI) {
    return handleCgi(connManager, configHandler, timerTree, sockfd);
  }
  connManager.setFinalResponse(sockfd,
                               std::vector<unsigned char>(final_response.begin(), final_response.end()));

  // send開始するまでのtimout追加
  // cgiだったら紐づくclient_socketにタイマーを設定する
  int client = connManager.isCgiSocket(sockfd) ? connManager.getCgiHandler(sockfd).getCliSocket() : sockfd;
  addTimerByType(connManager, configHandler, timerTree, client, Timer::TMO_KEEPALIVE);

  connManager.setEvent(sockfd, ConnectionData::EV_WRITE);  // writeイベントに更新
  return RequestHandler::UPDATE_WRITE;
}

int RequestHandler::handleCgi(ConnectionManager &connManager, ConfigHandler &configHandler,
                              TimerTree &timerTree, const int sockfd) {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);

  bool re = connManager.callCgiExecutor(sockfd, response, request);
  if (!re) {
    connManager.resetCgiSockets(sockfd);
    response.state_ = HttpResponse::RES_CGI_ERROR;
    return handleResponse(connManager, configHandler, timerTree, sockfd);
  }
  // bodyが空なら、bodyをsendしない
  int cgi_sock = connManager.getCgiHandler(sockfd).getCgiSocket();
  if (request.body.empty()) {
    connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_READ);
    // 最初にcgiのレスポンスをrecvするまでのtimeout
    addTimerByType(connManager, configHandler, timerTree, cgi_sock, Timer::TMO_RECV);
    return RequestHandler::UPDATE_CGI_READ;
  }
  connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_WRITE);
  // 最初にcgiにbodyをsendするまでのtimeout
  addTimerByType(connManager, configHandler, timerTree, cgi_sock, Timer::TMO_SEND);
  return RequestHandler::UPDATE_CGI_WRITE;
}

int RequestHandler::handleCgiReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                       ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  ssize_t re = ioHandler.receiveCgiResponse(connManager, sockfd);
  int status = -1;
  // recv error
  if (re == -1) return RequestHandler::UPDATE_NONE;
  const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);

  if (!cgiProcessExited(cgi_handler.getCgiProcessId(), status)) {
    addTimerByType(connManager, configHandler, timerTree, sockfd,
                   Timer::TMO_RECV);  // cgiからrecvする間のtimeout
    return RequestHandler::UPDATE_NONE;
  }
  if (!WEXITSTATUS(status)) {  // 正常にcgiが終了した場合
    const std::vector<unsigned char> &v = connManager.getCgiResponse(sockfd);
    std::string res(v.begin(), v.end());
    bool parse_suc = connManager.callCgiParser(sockfd, response, res);
    response.state_ = parse_suc ? HttpResponse::RES_PARSED_CGI : HttpResponse::RES_CGI_ERROR;
  } else {  // cgiが異常終了した場合
    response.state_ = HttpResponse::RES_CGI_EXIT_FAILURE;
  }

  re = handleResponse(connManager, configHandler, timerTree, sockfd);
  ioHandler.closeConnection(connManager, timerTree, sockfd);  // delete cgi event
  return re;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                     ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd) {
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
    return handleCgiWriteEvent(ioHandler, connManager, configHandler, timerTree, sockfd);

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
    addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_SEND);
    return RequestHandler::UPDATE_NONE;
  }

  const HttpResponse &response = connManager.getResponse(sockfd);
  if (!HttpResponse::isKeepaliveConnection(response)) {
    ioHandler.closeConnection(connManager, timerTree, sockfd);
    return UPDATE_CLOSE;
  }
  const std::vector<unsigned char> &context = connManager.getRawRequest(sockfd);
  // requestが残っている場合は、引き続きparseする
  if (!context.empty()) {
    connManager.clearResData(sockfd);
    addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_RECV);
    return handleRequest(connManager, configHandler, timerTree, sockfd);
  }
  addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_KEEPALIVE);

  // readイベントに更新
  connManager.setEvent(sockfd, ConnectionData::EV_READ);
  // connection dataを削除
  connManager.clearConnectionData(sockfd);
  return RequestHandler::UPDATE_READ;
}

int RequestHandler::handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        ConfigHandler &configHandler, TimerTree &timerTree,
                                        const int sockfd) {
  ioHandler.sendRequestBody(connManager, sockfd);

  const std::string body = connManager.getRequest(sockfd).body;
  const cgi::CGIHandler cgi_handler = connManager.getCgiHandler(sockfd);
  int status = -1;
  if (connManager.getSentBytes(sockfd) == body.size() ||          // bodyを全て送ったら
      cgiProcessExited(cgi_handler.getCgiProcessId(), status)) {  // cgi processがすでに死んでいたら
    connManager.resetSentBytes(sockfd);
    connManager.setEvent(sockfd, ConnectionData::EV_CGI_READ);
    addTimerByType(connManager, configHandler, timerTree, sockfd,
                   Timer::TMO_RECV);  // cgiのレスポンスをreadするまでのtimeout
    return RequestHandler::UPDATE_CGI_READ;
  }
  addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_SEND);  // cgiにsendする間のtimeout
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
  // cgiならすぐには接続切らず、timoutに任せる
  if (connManager.isCgiSocket(sockfd)) return RequestHandler::UPDATE_NONE;
  ioHandler.closeConnection(connManager, timerTree, sockfd);
  return RequestHandler::UPDATE_CLOSE;
}

std::map<int, RequestHandler::UPDATE_STATUS> RequestHandler::handleTimeoutEvent(
    NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler,
    TimerTree &timerTree) {
  std::map<int, RequestHandler::UPDATE_STATUS> timeout_sock_map;
  // timeoutしていない最初のイテレータを取得
  Timer current_time(-1, 0);
  std::multiset<Timer>::iterator upper_bound = timerTree.getTimerTree().upper_bound(current_time);

  // timeout している接続をすべて削除
  for (std::multiset<Timer>::iterator it = timerTree.getTimerTree().begin(); it != upper_bound;) {
    // next iterator を保存
    std::multiset<Timer>::iterator next = it;
    next++;
    // timer treeから削除
    if (connManager.isCgiSocket(it->getFd())) {
      int cgi_sock = it->getFd();
      const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(cgi_sock);
      int client_sock = cgi_handler.getCliSocket();

      cgi_handler.killCgiProcess();
      ioHandler.closeConnection(connManager, timerTree, cgi_sock);
      // 504 error responseを生成
      HttpResponse &response = connManager.getResponse(client_sock);
      response.state_ = HttpResponse::RES_CGI_TIMEOUT;
      handleResponse(connManager, configHandler, timerTree, client_sock);  // 中でsetEvent
      timeout_sock_map[client_sock] = RequestHandler::UPDATE_WRITE;        // epoll, kqueue用
      configHandler.writeErrorLog("webserv: [info] cgi timed out\n");      // debug
      it = next;
      continue;
    }
    ioHandler.closeConnection(connManager, timerTree, it->getFd());
    configHandler.writeErrorLog("webserv: [info] client timed out\n");  // debug
    it = next;
  }
  return timeout_sock_map;
}

/**
 * @brief cgi processが生きているか確認。死んでいたらstatusでexit status確認。
 *
 * @param process_id, status
 * @return true cgi processが死んでいる
 * @return false cgi processがまだ生きている
 */
bool RequestHandler::cgiProcessExited(const pid_t process_id, int &status) const {
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
 */
void RequestHandler::addTimerByType(ConnectionManager &connManager, ConfigHandler &configHandler,
                                    TimerTree &timerTree, const int sockfd, enum Timer::TimeoutType type) {
  config::Time timeout;
  std::map<std::string, std::string>::iterator it;

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

    case Timer::TMO_RECV:
      timeout = configHandler.searchReceiveTimeout(connManager.getTiedServer(sockfd), host_name,
                                                   connManager.getRequest(sockfd).uri);
      break;

    case Timer::TMO_SEND:
      timeout = configHandler.searchSendTimeout(connManager.getTiedServer(sockfd), host_name,
                                                connManager.getRequest(sockfd).uri);
      break;
  }

  // 設定値が0ならばタイムアウトを設定しないで削除
  if (timeout.isNoTime()) {
    timerTree.deleteTimer(sockfd);
    return;
  }

  timerTree.addTimer(Timer(sockfd, timeout));
}

bool RequestHandler::isOverWorkerConnections(ConnectionManager &connManager, ConfigHandler &configHandler) {
  return (connManager.getConnections().size() - connManager.getCgiSockNum()) >=
         configHandler.config_->events.worker_connections.getWorkerConnections();
}
