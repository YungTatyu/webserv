#ifndef POLL_SERVER_HPP
#define POLL_SERVER_HPP

#include <poll.h>

#include <vector>

#include "IServer.hpp"

class PollServer : public IServer {
 public:
  PollServer();
  ~PollServer();
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
  PollServer(const PollServer&);
  PollServer& operator=(const PollServer&);
  void addActiveEvents(const std::vector<struct pollfd>& pollfds, IActiveEventManager* event_manager);
  std::vector<struct pollfd> convertToPollfds(const ConnectionManager& conn_manager);
};

#endif
