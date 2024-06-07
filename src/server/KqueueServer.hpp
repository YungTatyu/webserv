#ifndef KQUEUE_SERVER_HPP
#define KQUEUE_SERVER_HPP

#include "IServer.hpp"
#include "conf.hpp"

#if defined(KQUEUE_AVAILABLE)

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <vector>

#include "KqueueActiveEventManager.hpp"

class KqueueServer : public IServer {
 public:
  KqueueServer();
  ~KqueueServer();
  void eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                 NetworkIOHandler* io_handler, RequestHandler* request_handler, ConfigHandler* config_handler,
                 TimerTree* timer_tree);
  int waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                   TimerTree* timer_tree);
  void callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                        NetworkIOHandler* io_handler, RequestHandler* request_handler,
                        ConfigHandler* config_handler, TimerTree* timer_tree);

 private:
  int kq_;  // kqueue fd
  bool initKqueueServer();
  bool initKevents(const std::map<int, ConnectionData*>& connections);
  int updateEvent(struct kevent& event, const short event_filter);
  int deleteEvent(struct kevent& event);
  int addNewEvent(const int fd, const short event_filter);
  void addClientNewEvent(const std::map<int, RequestHandler::UPDATE_STATUS>& timeout_sock_map);
};

#endif
#endif
