#include <cstring>

#include "WebServer.hpp"
#include "error.hpp"
#if defined(EPOLL_AVAILABLE)
#include <sys/epoll.h>

#include "EpollActiveEventManager.hpp"
#include "EpollServer.hpp"

EpollServer::EpollServer() {}

EpollServer::~EpollServer() { close(this->epfd_); }

void EpollServer::eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                            NetworkIOHandler* io_handler, TimerTree* timer_tree) {
  if (!initEpollServer()) return;

  if (!initEpollEvent(conn_manager->getConnections())) return;

  for (;;) {
    waitForEvent(conn_manager, event_manager, timer_tree);

    // 発生したイベントをhandle
    callEventHandler(conn_manager, event_manager, io_handler, timer_tree);

    // 発生したすべてのイベントを削除
    event_manager->clearAllEvents();
  }
}

bool EpollServer::initEpollServer() {
  const ConfigHandler& config_handler = WebServer::getConfigHandler();
  // epoll instance 初期化
  // epoll1_create(EPOLL_CLOEXEC);を使用することで、forkでこのfdのオープンを防げる。
  this->epfd_ = epoll_create(config_handler.getWorkerConnections());
  if (this->epfd_ == -1) {
    std::cerr << error::strSysCallError("epoll_create") << "\n";
    return false;
  }
  return true;
}

bool EpollServer::initEpollEvent(const std::map<int, ConnectionData*>& connections) {
  // 監視したいイベントを追加
  for (std::map<int, ConnectionData*>::const_iterator it = connections.begin(); it != connections.end();
       ++it) {
    struct epoll_event ep;
    ep.events = it->second->event == ConnectionData::EV_READ ? EPOLLIN : EPOLLOUT;
    ep.data.fd = it->first;

    if (epoll_ctl(this->epfd_, EPOLL_CTL_ADD, ep.data.fd, &ep) == -1) {
      std::cerr << error::strSysCallError("epoll_ctl") << "\n";
      return false;
    }
  }

  return true;
}

void EpollServer::callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                                   NetworkIOHandler* io_handler, TimerTree* timer_tree) {
  // event handling
  std::vector<struct epoll_event>* active_events_ptr =
      static_cast<std::vector<struct epoll_event>*>(event_manager->getActiveEvents());
  std::vector<struct epoll_event>& active_events = *active_events_ptr;
  const RequestHandler& request_handler = WebServer::getRequestHandler();

  // 現在時刻を更新
  Timer::updateCurrentTime();

  // TimeoutEvent発生
  if (event_manager->getActiveEventsNum() == 0) {
    request_handler.handleTimeoutEvent(*io_handler, *conn_manager, this, *timer_tree);
    return;
  }

  // 発生したイベントの数だけloopする
  for (int i = 0; i < event_manager->getActiveEventsNum(); ++i) {
    // 他のイベントハンドラーにconnectionが切断される可能性がある
    if (conn_manager->isClosedConnection(active_events[i].data.fd)) continue;
    if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleReadEvent(*io_handler, *conn_manager, this, *timer_tree,
                                      active_events[i].data.fd);
    else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleWriteEvent(*io_handler, *conn_manager, this, *timer_tree,
                                       active_events[i].data.fd);
    else if (event_manager->isEofEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleEofEvent(*io_handler, *conn_manager, this, *timer_tree, active_events[i].data.fd);
    else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleErrorEvent(*io_handler, *conn_manager, this, *timer_tree,
                                       active_events[i].data.fd);
  }
  request_handler.handleTimeoutEvent(*io_handler, *conn_manager, this, *timer_tree);
  conn_manager->clearClosedConnections();
}

int EpollServer::waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                              TimerTree* timer_tree) {
  // ここでタイムアウトを設定する必要があるならば、
  // epoll_pwait2()を使うことで、timespec型のtimeout値を指定できる。
  // おそらく必要ないので、epoll_wait()でtimeoutは-1を指定
  // epoll_waitが返すエラー
  // EINTR -> signal
  // EFAULT -> eventsによって指し示されたメモリが書き込み権限無し、またはメモリ不足。この場合再実行？
  std::vector<struct epoll_event>* active_events =
      static_cast<std::vector<struct epoll_event>*>(event_manager->getActiveEvents());

  active_events->resize(conn_manager->getConnections().size());

  // 現在時刻を更新
  Timer::updateCurrentTime();
  int size = epoll_wait(this->epfd_, active_events->data(), active_events->size(), timer_tree->findTimer());
  event_manager->setActiveEventsNum(size);
  if (size == -1) WebServer::writeErrorlog(error::strSysCallError("epoll_wait") + "\n");
  return size;
}

int EpollServer::addNewEvent(int fd, ConnectionData::EVENT event) {
  struct epoll_event new_event;
  new_event.events =
      event == ConnectionData::EV_READ || event == ConnectionData::EV_CGI_READ ? EPOLLIN : EPOLLOUT;
  new_event.data.fd = fd;
  int re = retryEpollCtl(EPOLL_CTL_ADD, new_event.data.fd, &new_event);
  return re;
}

int EpollServer::updateEvent(int fd, ConnectionData::EVENT event) {
  struct epoll_event new_event;
  new_event.events =
      event == ConnectionData::EV_READ || event == ConnectionData::EV_CGI_READ ? EPOLLIN : EPOLLOUT;
  new_event.data.fd = fd;
  int re = retryEpollCtl(EPOLL_CTL_MOD, new_event.data.fd, &new_event);
  return re;
}

int EpollServer::deleteEvent(int fd, ConnectionData::EVENT event) {
  static_cast<void>(event);
  int re = retryEpollCtl(EPOLL_CTL_DEL, fd, NULL);
  return re;
}

int EpollServer::retryEpollCtl(int op, int fd, struct epoll_event* event) {
  int ret;
  for (int i = 0; i < kRetry; ++i) {
    ret = epoll_ctl(this->epfd_, op, fd, event);
    if (ret != -1) break;
    // 起こりうるのはENOMEMかENOSPC
    WebServer::writeErrorlog(error::strSysCallError("epoll_ctl") + "\n");
  }
  return ret;
}

#endif
