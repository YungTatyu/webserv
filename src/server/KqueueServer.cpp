#include "KqueueServer.hpp"

#include "ConfigHandler.hpp"
#include "ConnectionManager.hpp"
#include "IServer.hpp"
#include "RequestHandler.hpp"
#include "WebServer.hpp"
#include "error.hpp"
#if defined(KQUEUE_AVAILABLE)

#include <stdio.h>

KqueueServer::KqueueServer() {}

KqueueServer::~KqueueServer() { close(this->kq_); }

void KqueueServer::eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                             NetworkIOHandler* io_handler, IServer* server, TimerTree* timer_tree) {
  if (!initKqueueServer()) return;
  if (!initKevents(conn_manager->getConnections())) return;
  for (;;) {
    waitForEvent(conn_manager, event_manager, timer_tree);

    // 発生したイベントをhandleする
    callEventHandler(conn_manager, event_manager, io_handler, server, timer_tree);

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
    const int event_filter = it->second->event == ConnectionData::EV_READ ? EVFILT_READ : EVFILT_WRITE;
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

int KqueueServer::waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                               TimerTree* timer_tree) {
  std::vector<struct kevent>* active_events =
      static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());

  // 発生したeventをすべて格納できるサイズにする
  active_events->resize(conn_manager->getConnections().size());
  // 現在時刻を更新
  Timer::updateCurrentTime();
  // TODO: error処理どうするか？ server downさせる？
  struct timespec ts = timer_tree->findTimespec();
  struct timespec* tsp = &ts;
  if (ts.tv_sec == -1 && ts.tv_nsec == -1) tsp = NULL;
  int re = kevent(this->kq_, NULL, 0, active_events->data(), active_events->size(), tsp);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("kevent") + "\n");
  event_manager->setActiveEventsNum(re);
  return re;
}

/*
 * cgi socketがtimeoutした場合にそれに紐づくclientのeventをWRITE EVENTに変更する関数。
 */
void KqueueServer::addClientNewEvent(const std::map<int, RequestHandler::UPDATE_STATUS>& timeout_sock_map) {
  std::map<int, RequestHandler::UPDATE_STATUS>::const_iterator it;

  for (it = timeout_sock_map.begin(); it != timeout_sock_map.end(); ++it) {
    switch (it->second) {
      case RequestHandler::UPDATE_WRITE:
        // addNewEvent(it->first, EVFILT_WRITE);
        break;

      default:
        break;
    }
  }
}

void KqueueServer::callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                                    NetworkIOHandler* io_handler, IServer* server, TimerTree* timer_tree) {
  std::vector<struct kevent>* active_events_ptr =
      static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());
  std::vector<struct kevent>& active_events = *active_events_ptr;
  std::map<int, RequestHandler::UPDATE_STATUS> timeout_sock_map;
  const RequestHandler& request_handler = WebServer::getRequestHandler();

  // 現在時刻を更新
  Timer::updateCurrentTime();

  // TimeoutEvent発生
  if (event_manager->getActiveEventsNum() == 0) {
    timeout_sock_map = request_handler.handleTimeoutEvent(*io_handler, *conn_manager, server, *timer_tree);
    addClientNewEvent(timeout_sock_map);
    return;
  }

  // 発生したイベントの数だけloopする
  for (int i = 0; i < event_manager->getActiveEventsNum(); ++i) {
    /**
     * 以下の変数は、event handlerを呼ぶ前に取得する必要がある
     * event handlerでcgiのコネクションはクローズされる場合があるため
     */
    // const cgi::CGIHandler& cgi_handler = conn_manager->getCgiHandler(active_events[i].ident);
    // const bool is_cgi_sock = conn_manager->isCgiSocket(active_events[i].ident);
    // const int cli_sock = cgi_handler.getCliSocket();  // cgi socketの場合に使用する
    //
    // TODO: kqueueはeofもevnetとして感知できるので、最適化の余地あり
    // int status = RequestHandler::UPDATE_NONE;
    if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleReadEvent(*io_handler, *conn_manager, server, *timer_tree,
                                      active_events[i].ident);
    else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleWriteEvent(*io_handler, *conn_manager, server, *timer_tree,
                                       active_events[i].ident);
    else if (event_manager->isEofEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleEofEvent(*io_handler, *conn_manager, server, *timer_tree, active_events[i].ident);
    else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
      request_handler.handleErrorEvent(*io_handler, *conn_manager, server, *timer_tree,
                                       active_events[i].ident);

    // kqueueで監視しているイベント情報を更新
    //   switch (status) {
    //     case RequestHandler::UPDATE_READ:
    //       updateEvent(active_events[i], EVFILT_READ);
    //       break;
    //     case RequestHandler::UPDATE_WRITE:
    //       if (is_cgi_sock) {
    //         // deleteEvent(active_events[i]); // cgi socketを監視から削除する: cgi
    //         // socketをcloseした後だから呼ばなくてもいい
    //         addNewEvent(cli_sock, EVFILT_WRITE);
    //         break;
    //       }
    //       updateEvent(active_events[i], EVFILT_WRITE);
    //       break;
    //     case RequestHandler::UPDATE_CLOSE:
    //       // deleteEvent(active_events[i]); // socketをcloseした後だから呼ばなくてもいい
    //       break;
    //     case RequestHandler::UPDATE_CGI_READ:
    //       if (is_cgi_sock) {
    //         updateEvent(active_events[i], EVFILT_READ);
    //         break;
    //       }
    //       deleteEvent(active_events[i]);  // client socketを監視から一時的に削除する
    //       addNewEvent(cgi_handler.getCgiSocket(), EVFILT_READ);
    //       break;
    //     case RequestHandler::UPDATE_CGI_WRITE:
    //       deleteEvent(active_events[i]);  // client socketを監視から一時的に削除する
    //       addNewEvent(cgi_handler.getCgiSocket(), EVFILT_WRITE);
    //       break;
    //     default:
    //       if (status >= 0)  // fdだったら
    //         addNewEvent(status, EVFILT_READ);
    //       break;
    //   }
  }
  timeout_sock_map = request_handler.handleTimeoutEvent(*io_handler, *conn_manager, server, *timer_tree);
  addClientNewEvent(timeout_sock_map);
}

int KqueueServer::addNewEvent(int fd, ConnectionData::EVENT event) {
  struct kevent kv;
  short filter =
      event == ConnectionData::EV_READ || event == ConnectionData::EV_CGI_READ ? EVFILT_READ : EVFILT_WRITE;
  EV_SET(&kv, fd, filter, EV_ADD | EV_ENABLE, 0, 0, 0);
  int re = kevent(this->kq_, &kv, 1, NULL, 0, NULL);
  // TODO: errorのとき、再トライ？
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("kevent") + "\n");
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
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("kevent") + "\n");
  return re;
}

#endif
