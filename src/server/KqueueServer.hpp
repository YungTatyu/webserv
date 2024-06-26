#ifndef KQUEUE_SERVER_HPP
#define KQUEUE_SERVER_HPP

#include "conf.hpp"
#if defined(KQUEUE_AVAILABLE)

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <vector>

#include "IServer.hpp"
#include "KqueueActiveEventManager.hpp"

class KqueueServer : public IServer {
 public:
  KqueueServer();
  ~KqueueServer();
  void eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                 NetworkIOHandler* io_handler, TimerTree* timer_tree);
  int waitForEvent(NetworkIOHandler* io_handler, ConnectionManager* conn_manager,
                   IActiveEventManager* event_manager, TimerTree* timer_tree);
  void callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                        NetworkIOHandler* io_handler, TimerTree* timer_tree);
  int addNewEvent(int fd, ConnectionData::EVENT event);
  int updateEvent(int fd, ConnectionData::EVENT event);
  int deleteEvent(int fd, ConnectionData::EVENT event);

 private:
  KqueueServer(const KqueueServer&);
  KqueueServer& operator=(const KqueueServer&);
  bool initKqueueServer();
  bool initKevents(const std::map<int, ConnectionData*>& connections);

  int kq_;  // kqueue fd
};

#endif
#endif
