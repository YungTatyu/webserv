#include "RequestHandler.hpp"

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "WebServer.hpp"

RequestHandler::RequestHandler() {}

void RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                     IServer *server, TimerTree &timerTree, const int sockfd) const {
  const ConfigHandler &configHandler = WebServer::getConfigHandler();
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_READ)
    return handleCgiReadEvent(ioHandler, connManager, server, timerTree, sockfd);
  // リスニングソケットへの新規リクエスト
  if (ioHandler.isListenSocket(sockfd)) {
    int new_sock = ioHandler.acceptConnection(connManager, sockfd);
    if (new_sock == -1) return;
    server->addNewEvent(new_sock, ConnectionData::EV_READ);
    connManager.setEvent(new_sock, ConnectionData::EV_READ);
    addTimerByType(connManager, configHandler, timerTree, new_sock, Timer::TMO_RECV);
    if (!isOverWorkerConnections(connManager, configHandler)) return;
    int timeout_fd = timerTree.getClosestTimeout();
    // TODO: cgiの時はどうする? nginxの場合は、新しいクライアントのリクエストを受け付けない
    // cgiのイベントを監視している場合は、cgiプロセスをkillしないといけない
    if (connManager.isCgiSocket(sockfd)) {
      const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(sockfd);
      cgi_handler.killCgiProcess();
    }
    // cgiの時は、clientとcgi両方削除しないといけない
    ioHandler.purgeConnection(connManager, server, timerTree, timeout_fd);

    return;
  }

  // クライアントソケットへのリクエスト（既存コネクション）
  ssize_t re = ioHandler.receiveRequest(connManager, sockfd);
  // ソケット使用不可。
  if (re == -1) return;
  if (re == 0)  // クライアントが接続を閉じる
  {
    ioHandler.closeConnection(connManager, server, timerTree, sockfd);
    return;
  }
  // 2つのrecv間のtimeout設定
  addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_RECV);
  return handleRequest(connManager, configHandler, server, timerTree, sockfd);
}

void RequestHandler::handleRequest(ConnectionManager &connManager, const ConfigHandler &configHandler,
                                   IServer *server, TimerTree &timerTree, const int sockfd) const {
  const std::vector<unsigned char> &context = connManager.getRawRequest(sockfd);
  // reinterpret_cast<const char*>を使うと、文字の長さにバグが生じる
  std::string raw_request = std::string(context.begin(), context.end());
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpRequest::parseRequest(raw_request, request);

  // requestが未完の場合は引き続きrequestをまつ
  if (HttpRequest::isParsePending(request)) {
    connManager.clearRawRequest(sockfd);
    return;
  }
  // parseした分のraw requestを削除しなければいけない
  // 処理のオーバーヘッド？ 最適化の余地あり
  std::vector<unsigned char> v(raw_request.begin(), raw_request.end());
  connManager.setRawRequest(sockfd, v);
  return handleResponse(connManager, configHandler, server, timerTree, sockfd);
}

void RequestHandler::handleResponse(ConnectionManager &connManager, const ConfigHandler &configHandler,
                                    IServer *server, TimerTree &timerTree, const int sockfd) const {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);
  std::string final_response = HttpResponse::generateResponse(
      request, response, connManager.getTiedServer(sockfd), sockfd, configHandler);

  if (response.state_ == HttpResponse::RES_EXECUTE_CGI) {
    // client側のtimerを消してからcgiの処理へ進む
    timerTree.deleteTimer(sockfd);
    return handleCgi(connManager, configHandler, server, timerTree, sockfd);
  }
  connManager.setFinalResponse(sockfd,
                               std::vector<unsigned char>(final_response.begin(), final_response.end()));

  // send開始するまでのtimout追加
  // cgiだったら紐づくclient_socketにタイマーを設定する
  int client = connManager.isCgiSocket(sockfd) ? connManager.getCgiHandler(sockfd).getCliSocket() : sockfd;
  addTimerByType(connManager, configHandler, timerTree, client, Timer::TMO_KEEPALIVE);

  if (connManager.isCgiSocket(sockfd))  // cgi socketの場合は、クライアントをイベントとして登録する
    server->addNewEvent(client, ConnectionData::EV_WRITE);
  else
    server->updateEvent(sockfd, ConnectionData::EV_WRITE);
  connManager.setEvent(sockfd, ConnectionData::EV_WRITE);  // writeイベントに更新
}

void RequestHandler::handleCgi(ConnectionManager &connManager, const ConfigHandler &configHandler,
                               IServer *server, TimerTree &timerTree, const int sockfd) const {
  HttpRequest &request = connManager.getRequest(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);

  bool re = connManager.callCgiExecutor(sockfd, response, request);
  if (!re) {
    response.state_ = HttpResponse::RES_CGI_ERROR;
    return handleResponse(connManager, configHandler, server, timerTree, sockfd);
  }
  // bodyが空なら、bodyをsendしない
  int cgi_sock = connManager.getCgiHandler(sockfd).getCgiSocket();
  if (request.body.empty()) {
    server->deleteEvent(sockfd, connManager.getEvent(sockfd));
    server->addNewEvent(cgi_sock, ConnectionData::EV_CGI_READ);
    connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_READ);
    // 最初にcgiのレスポンスをrecvするまでのtimeout
    addTimerByType(connManager, configHandler, timerTree, cgi_sock, Timer::TMO_RECV);
    return;
  }
  server->deleteEvent(sockfd, connManager.getEvent(sockfd));
  server->addNewEvent(cgi_sock, ConnectionData::EV_CGI_WRITE);
  connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_WRITE);
  // 最初にcgiにbodyをsendするまでのtimeout
  addTimerByType(connManager, configHandler, timerTree, cgi_sock, Timer::TMO_SEND);
}

void RequestHandler::handleCgiReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        IServer *server, TimerTree &timerTree, const int sockfd) const {
  const ConfigHandler &configHandler = WebServer::getConfigHandler();
  ssize_t re = ioHandler.receiveCgiResponse(connManager, sockfd);
  int status = -1;
  // recv error
  if (re == -1) return;
  const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(sockfd);
  HttpResponse &response = connManager.getResponse(sockfd);

  if (re != 0 || !cgiProcessExited(cgi_handler.getCgiProcessId(), status)) {
    addTimerByType(connManager, configHandler, timerTree, sockfd,
                   Timer::TMO_RECV);  // cgiからrecvする間のtimeout
    return;
  }
  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {  // 正常にcgiが終了した場合
    const std::vector<unsigned char> &v = connManager.getCgiResponse(sockfd);
    std::string res(v.begin(), v.end());
    bool parse_suc = connManager.callCgiParser(sockfd, response, res);
    response.state_ = parse_suc ? HttpResponse::RES_PARSED_CGI : HttpResponse::RES_CGI_ERROR;
  } else {  // cgiが異常終了した場合
    response.state_ = HttpResponse::RES_CGI_EXIT_FAILURE;
  }
  handleResponse(connManager, configHandler, server, timerTree, sockfd);
  ioHandler.closeConnection(connManager, server, timerTree, sockfd);  // delete cgi event
}

void RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                      IServer *server, TimerTree &timerTree, const int sockfd) const {
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
    return handleCgiWriteEvent(ioHandler, connManager, server, timerTree, sockfd);

  ssize_t re = ioHandler.sendResponse(connManager, sockfd);
  const ConfigHandler &configHandler = WebServer::getConfigHandler();

  // error
  if (re == -1) return;
  // client connection closed
  if (re == 0) {
    ioHandler.closeConnection(connManager, server, timerTree, sockfd);
    return;
  }
  const std::vector<unsigned char> &final_response = connManager.getFinalResponse(sockfd);
  /**
   * non-blockingでsendした場合、responseを全て送り切っていなかったとしても、指定したbyte数よりも少ないbyteをsendする場合がある
   * この現象はsocketが詰まった時に起こる
   * ex) ssize_t re = send(sock, buff, 1024) => re could be 1 to 1024
   * なので、responseを全て送り切ったか必ず確認する必要がある
   */
  if (connManager.getSentBytes(sockfd) != final_response.size()) {
    addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_SEND);
    return;  // 引き続きresponseを送信
  }

  const HttpResponse &response = connManager.getResponse(sockfd);
  if (!HttpResponse::isKeepaliveConnection(response)) {
    ioHandler.closeConnection(connManager, server, timerTree, sockfd);
    return;
  }
  const std::vector<unsigned char> &context = connManager.getRawRequest(sockfd);
  // requestが残っている場合は、引き続きparseする
  if (!context.empty()) {
    connManager.clearResData(sockfd);
    addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_RECV);
    return handleRequest(connManager, configHandler, server, timerTree, sockfd);
  }
  addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_KEEPALIVE);

  // readイベントに更新
  server->updateEvent(sockfd, ConnectionData::EV_READ);
  connManager.setEvent(sockfd, ConnectionData::EV_READ);
  // connection dataを削除
  connManager.clearConnectionData(sockfd);
}

void RequestHandler::handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                         IServer *server, TimerTree &timerTree, const int sockfd) const {
  const ConfigHandler &configHandler = WebServer::getConfigHandler();
  ioHandler.sendRequestBody(connManager, sockfd);

  const std::string &body = connManager.getRequest(sockfd).body;
  const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(sockfd);
  int status = -1;
  if (connManager.getSentBytes(sockfd) != body.size() &&           // bodyをまだ送る必要がある
      !cgiProcessExited(cgi_handler.getCgiProcessId(), status)) {  // cgi processが生きている
    addTimerByType(connManager, configHandler, timerTree, sockfd,
                   Timer::TMO_SEND);  // cgiにsendする間のtimeout
    return;
  }
  connManager.resetSentBytes(sockfd);
  server->updateEvent(sockfd, ConnectionData::EV_CGI_READ);
  connManager.setEvent(sockfd, ConnectionData::EV_CGI_READ);
  // cgiのレスポンスをreadするまでのtimeout
  addTimerByType(connManager, configHandler, timerTree, sockfd, Timer::TMO_RECV);
}

void RequestHandler::handleEofEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                    IServer *server, TimerTree &timerTree, const int sockfd) const {
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_READ) {
    return handleCgiReadEvent(ioHandler, connManager, server, timerTree, sockfd);
  } else if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE) {
    server->updateEvent(sockfd, ConnectionData::EV_CGI_READ);
    connManager.setEvent(sockfd, ConnectionData::EV_CGI_READ);
    // cgiのレスポンスをreadするまでのtimeout
    addTimerByType(connManager, config_handler, timerTree, sockfd, Timer::TMO_RECV);
    return;
  }
  ioHandler.closeConnection(connManager, server, timerTree, sockfd);
}

void RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                      IServer *server, TimerTree &timerTree, const int sockfd) const {
  // cgiならすぐには接続切らず、timoutに任せる
  if (connManager.isCgiSocket(sockfd)) return;
  ioHandler.closeConnection(connManager, server, timerTree, sockfd);
}

void RequestHandler::handleTimeoutEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager,
                                        IServer *server, TimerTree &timerTree) const {
  const ConfigHandler &configHandler = WebServer::getConfigHandler();
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
      // 504 error responseを生成
      HttpResponse &response = connManager.getResponse(cgi_sock);
      response.state_ = HttpResponse::RES_CGI_TIMEOUT;
      handleResponse(connManager, configHandler, server, timerTree, cgi_sock);  // 中でsetEvent
      // timeoutしたcgiの処理
      const cgi::CGIHandler &cgi_handler = connManager.getCgiHandler(cgi_sock);
      cgi_handler.killCgiProcess();
      ioHandler.closeConnection(connManager, server, timerTree, cgi_sock);
      connManager.clearResData(cgi_handler.getCliSocket());
      configHandler.writeErrorLog("webserv: [info] cgi timed out\n");  // debug
      it = next;
      continue;
    }
    ioHandler.closeConnection(connManager, server, timerTree, it->getFd());
    configHandler.writeErrorLog("webserv: [info] client timed out\n");  // debug
    it = next;
  }
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
  if (re == 0) return false;
  // errorの時も子プロセスが存在しないと判断する
  return true;
}

/**
 * @brief TimerTypeと直前のresponseのヘッダーに従ってtimeout値を取得し、TimerTreeにtimerを追加する。
 *
 * @param NetworkIOHandler, ConnectionManager, ConfigHandler, TimerTree, socket, TimeoutType
 *
 */
void RequestHandler::addTimerByType(ConnectionManager &connManager, const ConfigHandler &configHandler,
                                    TimerTree &timerTree, const int sockfd,
                                    enum Timer::TimeoutType type) const {
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

bool RequestHandler::isOverWorkerConnections(ConnectionManager &connManager,
                                             const ConfigHandler &configHandler) const {
  return (connManager.getConnections().size() - connManager.getCgiSockNum()) >=
         configHandler.config_->events.worker_connections.getWorkerConnections();
}
