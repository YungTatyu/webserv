#include "EventHandler.hpp"

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "WebServer.hpp"

EventHandler::EventHandler() {}

EventHandler::~EventHandler() {}

void EventHandler::handleReadEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                   IServer *server, TimerTree &timer_tree, int sock) const {
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  if (conn_manager.getEvent(sock) == ConnectionData::EV_CGI_READ)
    return handleCgiReadEvent(io_handler, conn_manager, server, timer_tree, sock);
  // リスニングソケットへの新規リクエスト
  if (io_handler.isListenSocket(sock)) {
    int new_sock = io_handler.acceptConnection(conn_manager, sock);
    if (new_sock == -1) return;
    if (server->addNewEvent(new_sock, ConnectionData::EV_READ) == -1) {
      // eventの追加に失敗したら接続を切る
      io_handler.closeConnection(conn_manager, server, timer_tree, new_sock);
      return;
    }
    conn_manager.setEvent(new_sock, ConnectionData::EV_READ);
    addTimerByType(conn_manager, config_handler, timer_tree, new_sock, Timer::TMO_RECV);

    if (!isOverWorkerConnections(conn_manager, config_handler)) return;

    config_handler.writeErrorLog(utils::toStr(config_handler.getWorkerConnections()) +
                                     " worker_connections are not enough, reusing connections",
                                 config::WARN);
    int timeout_fd = timer_tree.getClosestTimeout();
    // TODO: cgiの時はどうする? nginxの場合は、新しいクライアントのリクエストを受け付けない
    // cgiのイベントを監視している場合は、cgiプロセスをkillしないといけない
    if (conn_manager.isCgiSocket(sock)) {
      const cgi::CgiHandler &cgi_handler = conn_manager.getCgiHandler(sock);
      cgi_handler.killCgiProcess();
    }
    // cgiの時は、clientとcgi両方削除しないといけない
    io_handler.purgeConnection(conn_manager, server, timer_tree, timeout_fd);

    return;
  }

  // クライアントソケットへのリクエスト（既存コネクション）
  ssize_t re = io_handler.receiveRequest(conn_manager, sock);
  // ソケット使用不可。
  if (re == -1) return;
  if (re == 0)  // クライアントが接続を閉じる
  {
    io_handler.closeConnection(conn_manager, server, timer_tree, sock);
    return;
  }
  // 2つのrecv間のtimeout設定
  addTimerByType(conn_manager, config_handler, timer_tree, sock, Timer::TMO_RECV);
  return handleRequest(io_handler, conn_manager, config_handler, server, timer_tree, sock);
}

void EventHandler::handleRequest(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                 const ConfigHandler &config_handler, IServer *server, TimerTree &timer_tree,
                                 int sock) const {
  const std::vector<unsigned char> &context = conn_manager.getRawRequest(sock);
  // reinterpret_cast<const char*>を使うと、文字の長さにバグが生じる
  std::string raw_request = std::string(context.begin(), context.end());
  HttpRequest &request = conn_manager.getRequest(sock);
  HttpRequest::parseRequest(raw_request, request);

  // requestが未完の場合は引き続きrequestをまつ
  if (HttpRequest::isParsePending(request)) {
    conn_manager.clearRawRequest(sock);
    return;
  }
  // parseした分のraw requestを削除しなければいけない
  // 処理のオーバーヘッド？ 最適化の余地あり
  std::vector<unsigned char> v(raw_request.begin(), raw_request.end());
  conn_manager.setRawRequest(sock, v);
  return handleResponse(io_handler, conn_manager, config_handler, server, timer_tree, sock);
}

void EventHandler::handleResponse(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                  const ConfigHandler &config_handler, IServer *server, TimerTree &timer_tree,
                                  int sock) const {
  HttpRequest &request = conn_manager.getRequest(sock);
  HttpResponse &response = conn_manager.getResponse(sock);
  std::string final_response = HttpResponse::generateResponse(
      request, response, conn_manager.getTiedServer(sock), sock, config_handler);

  if (response.state_ == HttpResponse::RES_EXECUTE_CGI) {
    // client側のtimerを消してからcgiの処理へ進む
    timer_tree.deleteTimer(sock);
    return handleCgi(io_handler, conn_manager, config_handler, server, timer_tree, sock);
  }
  conn_manager.setFinalResponse(sock,
                                std::vector<unsigned char>(final_response.begin(), final_response.end()));

  // send開始するまでのtimout追加
  // cgiだったら紐づくclient_socketにタイマーを設定する
  int client = conn_manager.isCgiSocket(sock) ? conn_manager.getCgiHandler(sock).getCliSocket() : sock;
  addTimerByType(conn_manager, config_handler, timer_tree, client, Timer::TMO_KEEPALIVE);

  if (conn_manager.isCgiSocket(sock))  // cgi socketの場合は、クライアントをイベントとして登録する
    server->addNewEvent(client, ConnectionData::EV_WRITE);
  else
    server->updateEvent(sock, ConnectionData::EV_WRITE);
  conn_manager.setEvent(sock, ConnectionData::EV_WRITE);  // writeイベントに更新
}

void EventHandler::handleCgi(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                             const ConfigHandler &config_handler, IServer *server, TimerTree &timer_tree,
                             int sock) const {
  HttpRequest &request = conn_manager.getRequest(sock);
  HttpResponse &response = conn_manager.getResponse(sock);

  cgi::CgiHandler &cgi_handler = conn_manager.getCgiHandler(sock);
  bool re = cgi_handler.callCgiExecutor(response, request, sock);
  if (!re) {
    response.state_ = HttpResponse::RES_CGI_ERROR;
    return handleResponse(io_handler, conn_manager, config_handler, server, timer_tree, sock);
  }
  // bodyが空なら、bodyをsendしない
  int cgi_sock = conn_manager.getCgiHandler(sock).getCgiSocket();
  if (request.body_.empty()) {
    server->deleteEvent(sock, conn_manager.getEvent(sock));
    server->addNewEvent(cgi_sock, ConnectionData::EV_CGI_READ);
    // selectでFD_SETSIZE以上のfd値をセットしようとした時だけ失敗する
    if (!conn_manager.setCgiConnection(sock, ConnectionData::EV_CGI_READ)) {
      cgi_handler.killCgiProcess();
      io_handler.purgeConnection(conn_manager, server, timer_tree, sock);
      return;
    }
    // 最初にcgiのレスポンスをrecvするまでのtimeout
    addTimerByType(conn_manager, config_handler, timer_tree, cgi_sock, Timer::TMO_RECV);
    return;
  }
  server->deleteEvent(sock, conn_manager.getEvent(sock));
  server->addNewEvent(cgi_sock, ConnectionData::EV_CGI_WRITE);
  // selectでFD_SETSIZE以上のfd値をセットしようとした時だけ失敗する
  if (!conn_manager.setCgiConnection(sock, ConnectionData::EV_CGI_WRITE)) {
    cgi_handler.killCgiProcess();
    io_handler.purgeConnection(conn_manager, server, timer_tree, sock);
    return;
  }
  // 最初にcgiにbodyをsendするまでのtimeout
  addTimerByType(conn_manager, config_handler, timer_tree, cgi_sock, Timer::TMO_SEND);
}

void EventHandler::handleCgiReadEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                      IServer *server, TimerTree &timer_tree, int sock) const {
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  ssize_t re = io_handler.receiveCgiResponse(conn_manager, sock);
  int status = -1;
  // recv error
  if (re == -1) return;
  cgi::CgiHandler &cgi_handler = conn_manager.getCgiHandler(sock);
  HttpResponse &response = conn_manager.getResponse(sock);

  if (re != 0 || !cgiProcessExited(cgi_handler.getCgiProcessId(), status)) {
    addTimerByType(conn_manager, config_handler, timer_tree, sock,
                   Timer::TMO_RECV);  // cgiからrecvする間のtimeout
    return;
  }
  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {  // 正常にcgiが終了した場合
    const std::vector<unsigned char> &v = conn_manager.getCgiResponse(sock);
    std::string res(v.begin(), v.end());
    bool parse_suc = cgi_handler.callCgiParser(response, res);
    response.state_ = parse_suc ? HttpResponse::RES_PARSED_CGI : HttpResponse::RES_CGI_ERROR;
  } else {  // cgiが異常終了した場合
    response.state_ = HttpResponse::RES_CGI_EXIT_FAILURE;
  }
  handleResponse(io_handler, conn_manager, config_handler, server, timer_tree, sock);
  io_handler.closeConnection(conn_manager, server, timer_tree, sock);  // delete cgi event
}

void EventHandler::handleWriteEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                    IServer *server, TimerTree &timer_tree, int sock) const {
  if (conn_manager.getEvent(sock) == ConnectionData::EV_CGI_WRITE)
    return handleCgiWriteEvent(io_handler, conn_manager, server, timer_tree, sock);

  ssize_t re = io_handler.sendResponse(conn_manager, sock);
  const ConfigHandler &config_handler = WebServer::getConfigHandler();

  // error
  if (re == -1) return;
  // client connection closed
  if (re == 0) {
    io_handler.closeConnection(conn_manager, server, timer_tree, sock);
    return;
  }
  const std::vector<unsigned char> &final_response = conn_manager.getFinalResponse(sock);
  /**
   * non-blockingでsendした場合、responseを全て送り切っていなかったとしても、指定したbyte数よりも少ないbyteをsendする場合がある
   * この現象はsocketが詰まった時に起こる
   * ex) ssize_t re = send(sock, buff, 1024) => re could be 1 to 1024
   * なので、responseを全て送り切ったか必ず確認する必要がある
   */
  if (conn_manager.getSentBytes(sock) != final_response.size()) {
    addTimerByType(conn_manager, config_handler, timer_tree, sock, Timer::TMO_SEND);
    return;  // 引き続きresponseを送信
  }

  const HttpResponse &response = conn_manager.getResponse(sock);
  if (!HttpResponse::isKeepaliveConnection(response)) {
    io_handler.closeConnection(conn_manager, server, timer_tree, sock);
    return;
  }
  const std::vector<unsigned char> &context = conn_manager.getRawRequest(sock);
  // requestが残っている場合は、引き続きparseする
  if (!context.empty()) {
    conn_manager.clearResData(sock);
    addTimerByType(conn_manager, config_handler, timer_tree, sock, Timer::TMO_RECV);
    return handleRequest(io_handler, conn_manager, config_handler, server, timer_tree, sock);
  }
  addTimerByType(conn_manager, config_handler, timer_tree, sock, Timer::TMO_KEEPALIVE);

  // readイベントに更新
  server->updateEvent(sock, ConnectionData::EV_READ);
  conn_manager.setEvent(sock, ConnectionData::EV_READ);
  // connection dataを削除
  conn_manager.clearConnectionData(sock);
}

void EventHandler::handleCgiWriteEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                       IServer *server, TimerTree &timer_tree, int sock) const {
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  io_handler.sendRequestBody(conn_manager, sock);

  const std::string &body = conn_manager.getRequest(sock).body_;
  const cgi::CgiHandler &cgi_handler = conn_manager.getCgiHandler(sock);
  int status = -1;
  if (conn_manager.getSentBytes(sock) != body.size() &&            // bodyをまだ送る必要がある
      !cgiProcessExited(cgi_handler.getCgiProcessId(), status)) {  // cgi processが生きている
    addTimerByType(conn_manager, config_handler, timer_tree, sock,
                   Timer::TMO_SEND);  // cgiにsendする間のtimeout
    return;
  }
  conn_manager.resetSentBytes(sock);
  server->updateEvent(sock, ConnectionData::EV_CGI_READ);
  conn_manager.setEvent(sock, ConnectionData::EV_CGI_READ);
  // cgiのレスポンスをreadするまでのtimeout
  addTimerByType(conn_manager, config_handler, timer_tree, sock, Timer::TMO_RECV);
}

void EventHandler::handleEofEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                  IServer *server, TimerTree &timer_tree, int sock) const {
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  if (conn_manager.getEvent(sock) == ConnectionData::EV_CGI_READ) {
    return handleCgiReadEvent(io_handler, conn_manager, server, timer_tree, sock);
  } else if (conn_manager.getEvent(sock) == ConnectionData::EV_CGI_WRITE) {
    server->updateEvent(sock, ConnectionData::EV_CGI_READ);
    conn_manager.setEvent(sock, ConnectionData::EV_CGI_READ);
    // cgiのレスポンスをreadするまでのtimeout
    addTimerByType(conn_manager, config_handler, timer_tree, sock, Timer::TMO_RECV);
    return;
  }
  io_handler.closeConnection(conn_manager, server, timer_tree, sock);
}

void EventHandler::handleErrorEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                    IServer *server, TimerTree &timer_tree, int sock) const {
  // cgiならすぐには接続切らず、timoutに任せる
  if (conn_manager.isCgiSocket(sock)) return;
  io_handler.closeConnection(conn_manager, server, timer_tree, sock);
}

void EventHandler::handleTimeoutEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                                      IServer *server, TimerTree &timer_tree) const {
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  // timeoutしていない最初のイテレータを取得
  Timer current_time(-1, 0);
  std::multiset<Timer>::iterator upper_bound = timer_tree.getTimerTree().upper_bound(current_time);

  // timeout している接続をすべて削除
  for (std::multiset<Timer>::iterator it = timer_tree.getTimerTree().begin(); it != upper_bound;) {
    // next iterator を保存
    std::multiset<Timer>::iterator next = it;
    next++;
    // timer treeから削除
    if (conn_manager.isCgiSocket(it->getFd())) {
      int cgi_sock = it->getFd();
      // 504 error responseを生成
      HttpResponse &response = conn_manager.getResponse(cgi_sock);
      response.state_ = HttpResponse::RES_CGI_TIMEOUT;
      handleResponse(io_handler, conn_manager, config_handler, server, timer_tree, cgi_sock);  // 中でsetEvent
      // timeoutしたcgiの処理
      const cgi::CgiHandler &cgi_handler = conn_manager.getCgiHandler(cgi_sock);
      cgi_handler.killCgiProcess();
      io_handler.closeConnection(conn_manager, server, timer_tree, cgi_sock);
      conn_manager.clearResData(cgi_handler.getCliSocket());
      config_handler.writeErrorLog("cgi timed out", config::DEBUG);  // debug
      it = next;
      continue;
    }
    io_handler.closeConnection(conn_manager, server, timer_tree, it->getFd());
    config_handler.writeErrorLog("client timed out", config::DEBUG);  // debug
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
bool EventHandler::cgiProcessExited(const pid_t process_id, int &status) const {
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
void EventHandler::addTimerByType(ConnectionManager &conn_manager, const ConfigHandler &config_handler,
                                  TimerTree &timer_tree, int sock, enum Timer::TimeoutType type) const {
  config::Time timeout;
  std::map<std::string, std::string>::iterator it;

  // Hostヘッダーがあるか確認
  // requestを処理する前にreceive_timeoutをセットするのでこの処理は必要。
  it = conn_manager.getRequest(sock).headers_.find("Host");
  std::string host_name;
  if (it == conn_manager.getRequest(sock).headers_.end())
    host_name = "";
  else
    host_name = it->second;

  // timeout時間セット
  switch (type) {
    case Timer::TMO_KEEPALIVE:
      timeout = config_handler.searchKeepaliveTimeout(conn_manager.getTiedServer(sock), host_name,
                                                      conn_manager.getRequest(sock).uri_);
      break;

    case Timer::TMO_RECV:
      timeout = config_handler.searchReceiveTimeout(conn_manager.getTiedServer(sock), host_name,
                                                    conn_manager.getRequest(sock).uri_);
      break;

    case Timer::TMO_SEND:
      timeout = config_handler.searchSendTimeout(conn_manager.getTiedServer(sock), host_name,
                                                 conn_manager.getRequest(sock).uri_);
      break;
  }

  // 設定値が0ならばタイムアウトを設定しないで削除
  if (timeout.isNoTime()) {
    timer_tree.deleteTimer(sock);
    return;
  }

  timer_tree.addTimer(Timer(sock, timeout));
}

bool EventHandler::isOverWorkerConnections(ConnectionManager &conn_manager,
                                           const ConfigHandler &config_handler) const {
  return (conn_manager.getConnections().size() - conn_manager.getCgiSockNum()) >=
         config_handler.getWorkerConnections();
}
