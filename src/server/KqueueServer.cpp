#include "KqueueServer.hpp"

#include "WebServer.hpp"
#include "error.hpp"
#if defined(KQUEUE_AVAILABLE)

#include <stdio.h>

KqueueServer::KqueueServer() {}

KqueueServer::~KqueueServer() { close(this->kq_); }

void KqueueServer::eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                             NetworkIOHandler* io_handler, TimerTree* timer_tree) {
  if (!initKqueueServer()) return;
  if (!initKevents(conn_manager->getConnections())) return;
  for (;;) {
    waitForEvent(io_handler, conn_manager, event_manager, timer_tree);

    // 発生したイベントをhandleする
    callEventHandler(conn_manager, event_manager, io_handler, timer_tree);

    // 発生したすべてのイベントを削除
    event_manager->clearAllEvents();
  }
}

bool KqueueServer::initKqueueServer() {
  this->kq_ = kqueue();
  if (this->kq_ == -1) {
    std::cerr << error::strSysCallError("kqueue") << "\n";
    return false;
  }
  return true;
}

bool KqueueServer::initKevents(const std::map<int, ConnectionData*>& connections) {
  std::vector<struct kevent> event_list;  // 監視したいevent

  // 監視するeventの数だけ、要素を確保する
  event_list.resize(connections.size());

  size_t i = 0;
  for (std::map<int, ConnectionData*>::const_iterator it = connections.begin(); it != connections.end();
       ++it) {
    const int event_filter = it->second->event_ == ConnectionData::EV_READ ? EVFILT_READ : EVFILT_WRITE;
    EV_SET(&event_list[i], it->first, event_filter, EV_ADD | EV_ENABLE, 0, 0, 0);
    ++i;
  }
  int re = kevent(this->kq_, event_list.data(), event_list.size(), NULL, 0, NULL);
  if (re == -1) {
    std::cerr << error::strSysCallError("kevent") << "\n";
    return false;
  }
  return true;
}

int KqueueServer::waitForEvent(NetworkIOHandler* io_handler, ConnectionManager* conn_manager,
                               IActiveEventManager* event_manager, TimerTree* timer_tree) {
  (void)io_handler;
  std::vector<struct kevent>* active_events =
      static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());

  // 発生したeventをすべて格納できるサイズにする
  active_events->resize(conn_manager->getConnections().size());
  // 現在時刻を更新
  Timer::updateCurrentTime();

  struct timespec ts = timer_tree->findTimespec();
  struct timespec* tsp = &ts;
  if (ts.tv_sec == -1 && ts.tv_nsec == -1) tsp = NULL;
  int re = kevent(this->kq_, NULL, 0, active_events->data(), active_events->size(), tsp);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("kevent") + "\n", config::EMERG);
  event_manager->setActiveEventsNum(re);
  return re;
}

void KqueueServer::callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                                    NetworkIOHandler* io_handler, TimerTree* timer_tree) {
  std::vector<struct kevent>* active_events_ptr =
      static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());
  std::vector<struct kevent>& active_events = *active_events_ptr;
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
    if (conn_manager->isClosedConnection(active_events[i].ident)) continue;
    if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleReadEvent(*io_handler, *conn_manager, this, *timer_tree, active_events[i].ident);
    else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleWriteEvent(*io_handler, *conn_manager, this, *timer_tree, active_events[i].ident);
    else if (event_manager->isEofEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleEofEvent(*io_handler, *conn_manager, this, *timer_tree, active_events[i].ident);
    else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleErrorEvent(*io_handler, *conn_manager, this, *timer_tree, active_events[i].ident);
  }
  request_handler.handleTimeoutEvent(*io_handler, *conn_manager, this, *timer_tree);
  conn_manager->clearClosedConnections();
}

int KqueueServer::addNewEvent(int fd, ConnectionData::EVENT event) {
  struct kevent kv;
  short filter =
      event == ConnectionData::EV_READ || event == ConnectionData::EV_CGI_READ ? EVFILT_READ : EVFILT_WRITE;
  EV_SET(&kv, fd, filter, EV_ADD | EV_ENABLE, 0, 0, 0);
  int re = kevent(this->kq_, &kv, 1, NULL, 0, NULL);
  // 起こりうるのはENOMEM
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("kevent") + "\n", config::EMERG);
  return re;
}

int KqueueServer::updateEvent(int fd, ConnectionData::EVENT event) {
  // 新しいイベントがreadならwriteイベントを削除
  // 逆も然り
  ConnectionData::EVENT old_event = event == ConnectionData::EV_READ || event == ConnectionData::EV_CGI_READ
                                        ? ConnectionData::EV_WRITE
                                        : ConnectionData::EV_READ;

  deleteEvent(fd, old_event);
  return addNewEvent(fd, event);
}

int KqueueServer::deleteEvent(int fd, ConnectionData::EVENT event) {
  struct kevent kv;
  short filter =
      event == ConnectionData::EV_READ || event == ConnectionData::EV_CGI_READ ? EVFILT_READ : EVFILT_WRITE;
  EV_SET(&kv, fd, filter, EV_DELETE, 0, 0, 0);
  int re = kevent(this->kq_, &kv, 1, NULL, 0, NULL);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("kevent") + "\n", config::EMERG);
  return re;
}

#endif
