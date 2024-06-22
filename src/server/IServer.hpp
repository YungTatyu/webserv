#ifndef ISERVER_HPP
#define ISERVER_HPP

#include "ConnectionManager.hpp"
#include "IActiveEventManager.hpp"
#include "IServer.hpp"
#include "NetworkIOHandler.hpp"
#include "TimerTree.hpp"

class IServer {
 public:
  virtual ~IServer(){};
  virtual void eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                         NetworkIOHandler* io_handler, TimerTree* timer_tree) = 0;
  virtual int waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                           TimerTree* timer_tree) = 0;
  virtual void callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                                NetworkIOHandler* io_handler, TimerTree* timer_tree) = 0;
  virtual int addNewEvent(int fd, ConnectionData::EVENT event) = 0;
  virtual int updateEvent(int fd, ConnectionData::EVENT event) = 0;
  virtual int deleteEvent(int fd, ConnectionData::EVENT event) = 0;
};

#endif
