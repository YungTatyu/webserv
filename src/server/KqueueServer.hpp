#ifndef KQUEUE_SERVER_HPP
#define KQUEUE_SERVER_HPP

#include "IServer.hpp"
#include "RequestHandler.hpp"
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
                 NetworkIOHandler* io_handler, TimerTree* timer_tree);
  int waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                   TimerTree* timer_tree);
  void callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                        NetworkIOHandler* io_handler, TimerTree* timer_tree);
  int addNewEvent(int fd, ConnectionData::EVENT event);
  int updateEvent(int fd, ConnectionData::EVENT event);
  int deleteEvent(int fd, ConnectionData::EVENT event);

 private:
  int kq_;  // kqueue fd
  const static int kRetry = 5;
  bool initKqueueServer();
  bool initKevents(const std::map<int, ConnectionData*>& connections);
  int retryKevent(const struct kevent* changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec* timeout);
};

#endif
#endif
