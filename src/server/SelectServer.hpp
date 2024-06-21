#ifndef SELECT_SERVER_HPP
#define SELECT_SERVER_HPP

#include <sys/select.h>

#include "ConnectionManager.hpp"
#include "IServer.hpp"
#include "NetworkIOHandler.hpp"
#include "RequestHandler.hpp"
#include "SelectActiveEventManager.hpp"

class SelectServer : public IServer {
 public:
  SelectServer();
  ~SelectServer();
  void eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                 NetworkIOHandler* io_handler, IServer* server, TimerTree* timer_tree);
  int waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                   TimerTree* timer_tree);
  void callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                        NetworkIOHandler* io_handler, IServer* server, TimerTree* timer_tree);
  int addNewEvent(int fd, ConnectionData::EVENT event);
  int updateEvent(int fd, ConnectionData::EVENT event);
  int deleteEvent(int fd, ConnectionData::EVENT event);

 private:
  fd_set read_set_;
  fd_set write_set_;
  int addSocketToSets(const ConnectionManager& conn_manager);
  void addActiveEvents(const std::map<int, ConnectionData*>& connections, IActiveEventManager* event_manager);
};

#endif
