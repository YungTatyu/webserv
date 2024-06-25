#ifndef EPOLL_SERVER_HPP
#define EPOLL_SERVER_HPP

#if defined(EPOLL_AVAILABLE)

#include "IServer.hpp"

class ConnectionManager;
class NetworkIOHandler;
class RequestHandler;

class EpollServer : public IServer {
 public:
  EpollServer();
  ~EpollServer();
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
  int epfd_;  // epoll instance
  const static int kRetry = 5;
  bool initEpollServer();
  bool initEpollEvent(const std::map<int, ConnectionData*>& connections);
  int retryEpollCtl(int op, int fd, struct epoll_event* event);
};

#endif
#endif
