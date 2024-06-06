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
                 NetworkIOHandler* io_handler, RequestHandler* request_handler, ConfigHandler* config_handler,
                 TimerTree* timer_tree);
  int waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                   TimerTree* timer_tree);
  void callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                        NetworkIOHandler* io_handler, RequestHandler* request_handler,
                        ConfigHandler* config_handler, TimerTree* timer_tree);
  int addSocketToSets(const std::map<int, ConnectionData>& connections);
  void addActiveEvents(const std::map<int, ConnectionData>& connections, IActiveEventManager* event_manager);

 private:
  int epfd_;  // epoll instance
  bool initEpollServer();
  bool initEpollEvent(const std::map<int, ConnectionData*>& connections);
  int addNewEvent(const int fd, const uint32_t event_filter);
  int updateEvent(struct epoll_event& old_event, const uint32_t event_filter);
  int deleteEvent(struct epoll_event& old_event);
  void  addClientNewEvent(const std::map<int, RequestHandler::UPDATE_STATUS>& timeout_sock_map);
};

#endif
#endif
