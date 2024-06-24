#ifndef EPOLL_ACTIVE_EVENT_MANAGER_HPP
#define EPOLL_ACTIVE_EVENT_MANAGER_HPP

#include "conf.hpp"

#if defined(EPOLL_AVAILABLE)

#include <vector>

#include "IActiveEventManager.hpp"

class EpollActiveEventManager : public IActiveEventManager {
 public:
  EpollActiveEventManager();
  ~EpollActiveEventManager();
  void *getActiveEvents();
  int getActiveEventsNum();
  void setActiveEventsNum(int num);
  void addEvent(const void *event);
  void clearAllEvents();
  bool isReadEvent(const void *event);
  bool isWriteEvent(const void *event);
  bool isErrorEvent(const void *event);
  bool isEofEvent(const void *event);

 private:
  std::vector<struct epoll_event> active_events_;
  int active_events_num_;
};

#endif
#endif
