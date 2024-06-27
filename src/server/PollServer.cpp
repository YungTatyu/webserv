#include "PollServer.hpp"

#include "WebServer.hpp"
#include "error.hpp"

PollServer::PollServer() {}

PollServer::~PollServer() {}

void PollServer::eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                           NetworkIOHandler* io_handler, TimerTree* timer_tree) {
  for (;;) {
    waitForEvent(io_handler, conn_manager, event_manager, timer_tree);

    // 発生したイベントをhandleする
    callEventHandler(conn_manager, event_manager, io_handler, timer_tree);

    // 発生したすべてのイベントを削除
    event_manager->clearAllEvents();
  }
}

int PollServer::waitForEvent(NetworkIOHandler* io_handler, ConnectionManager* conn_manager,
                             IActiveEventManager* event_manager, TimerTree* timer_tree) {
  int re;
  std::vector<pollfd> pollfds;
  while (1) {
    pollfds = convertToPollfds(*conn_manager);

    // 現在時刻を更新
    Timer::updateCurrentTime();

    re = poll(pollfds.data(), pollfds.size(), timer_tree->findTimer());
    if (re != -1) break;
    WebServer::writeErrorlog(error::strSysCallError("poll") + "\n");
    // 起こりうるのはENOMEM
    // 失敗したらtimeoutが近いクライアントを切断して、メモリを空ける。
    int timeout_fd = timer_tree->getClosestTimeout();
    if (timeout_fd == -1)  // timeout treeに一つもクライアントがいなかったら例外投げる
      throw std::runtime_error(error::strSysCallError("poll"));
    if (conn_manager->isCgiSocket(timeout_fd)) {
      const cgi::CgiHandler& cgi_handler = conn_manager->getCgiHandler(timeout_fd);
      cgi_handler.killCgiProcess();
    }
    io_handler->purgeConnection(*conn_manager, this, *timer_tree, timeout_fd);
  }
  // 発生したイベントをActiveEventManagerにすべて追加
  addActiveEvents(pollfds, event_manager);
  return re;
}

void PollServer::addActiveEvents(const std::vector<pollfd>& pollfds, IActiveEventManager* event_manager) {
  for (size_t i = 0; i < pollfds.size(); ++i) {
    const struct pollfd& cur_pfd = pollfds[i];
    // イベントが発生していたら、active_eventに追加
    if (event_manager->isReadEvent(static_cast<const void*>(&cur_pfd)) ||
        event_manager->isWriteEvent(static_cast<const void*>(&cur_pfd)) ||
        event_manager->isEofEvent(static_cast<const void*>(&cur_pfd)) ||
        event_manager->isErrorEvent(static_cast<const void*>(&cur_pfd)))
      event_manager->addEvent(static_cast<const void*>(&cur_pfd));
  }
}

void PollServer::callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                                  NetworkIOHandler* io_handler, TimerTree* timer_tree) {
  const std::vector<pollfd>* active_events =
      static_cast<const std::vector<pollfd>*>(event_manager->getActiveEvents());
  const RequestHandler& request_handler = WebServer::getRequestHandler();

  // 現在時刻を更新
  Timer::updateCurrentTime();

  // TimeoutEvent発生
  if (event_manager->getActiveEventsNum() == 0) {
    request_handler.handleTimeoutEvent(*io_handler, *conn_manager, this, *timer_tree);
    return;
  }

  // 発生したイベントの数だけloopする
  for (std::vector<pollfd>::const_iterator it = active_events->begin(); it != active_events->end(); ++it) {
    // 他のイベントハンドラーにconnectionが切断される可能性がある
    if (conn_manager->isClosedConnection(it->fd)) continue;
    if (event_manager->isReadEvent(static_cast<const void*>(&(*it))))
      request_handler.handleReadEvent(*io_handler, *conn_manager, this, *timer_tree, it->fd);
    else if (event_manager->isWriteEvent(static_cast<const void*>(&(*it))))
      request_handler.handleWriteEvent(*io_handler, *conn_manager, this, *timer_tree, it->fd);
    else if (event_manager->isEofEvent(static_cast<const void*>(&(*it))))
      request_handler.handleEofEvent(*io_handler, *conn_manager, this, *timer_tree, it->fd);
    else if (event_manager->isErrorEvent(static_cast<const void*>(&(*it))))
      request_handler.handleErrorEvent(*io_handler, *conn_manager, this, *timer_tree, it->fd);
  }
  request_handler.handleTimeoutEvent(*io_handler, *conn_manager, this, *timer_tree);
  conn_manager->clearClosedConnections();
}

/**
 * @brief pollfdのvectorを作成する
 *
 * @param connections : すべてのクライアントソケットとそれにひもづくデータ
 * @return std::vector<struct pollfd>
 */
std::vector<struct pollfd> PollServer::convertToPollfds(const ConnectionManager& conn_manager) {
  std::vector<struct pollfd> list;
  const std::map<int, ConnectionData*>& connections = conn_manager.getConnections();
  for (std::map<int, ConnectionData*>::const_iterator it = connections.begin(); it != connections.end();
       ++it) {
    struct pollfd pollfd;
    switch (it->second->event_) {
      case ConnectionData::EV_CGI_READ:
      case ConnectionData::EV_CGI_WRITE:
        // cgi eventの時は、クライアントsocketはイベント登録しない
        if (!conn_manager.isCgiSocket(it->first)) continue;
        break;
      default:
        break;
    }
    pollfd.fd = it->first;
    switch (it->second->event_) {
      case ConnectionData::EV_READ:
      case ConnectionData::EV_CGI_READ:
        pollfd.events = POLLIN;
#if defined(__linux__)
        pollfd.events |= POLLRDHUP;
#endif
        break;
      case ConnectionData::EV_WRITE:
      case ConnectionData::EV_CGI_WRITE:
        pollfd.events = POLLOUT;
#if defined(__linux__)
        pollfd.events |= POLLRDHUP;
#endif
        break;
    }
    pollfd.revents = 0;
    list.push_back(pollfd);
  }
  return list;
}

int PollServer::addNewEvent(int fd, ConnectionData::EVENT event) {
  static_cast<void>(fd);
  static_cast<void>(event);
  return 0;
}

int PollServer::updateEvent(int fd, ConnectionData::EVENT event) {
  static_cast<void>(fd);
  static_cast<void>(event);
  return 0;
}

int PollServer::deleteEvent(int fd, ConnectionData::EVENT event) {
  static_cast<void>(fd);
  static_cast<void>(event);
  return 0;
}
