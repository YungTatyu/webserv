#ifndef KQUEUE_ACTIVE_EVENT_MANAGER_HPP
#define KQUEUE_ACTIVE_EVENT_MANAGER_HPP

#include "conf.hpp"

#if defined(KQUEUE_AVAILABLE)

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <vector>

#include "IActiveEventManager.hpp"

class KqueueActiveEventManager : public IActiveEventManager {
 public:
  KqueueActiveEventManager();
  ~KqueueActiveEventManager();
  void *getActiveEvents();
  int getActiveEventsNum();
  void setActiveEventsNum(const int num);
  void addEvent(const void *event);
  void clearAllEvents();
  bool isReadEvent(const void *event, const bool is_cgi_sock);
  bool isWriteEvent(const void *event, const bool is_cgi_sock);
  bool isErrorEvent(const void *event);
  bool isEofEvent(const void *event, const bool is_cgi_sock);

 private:
  std::vector<struct kevent> active_events_;
  int active_events_num_;
};

#endif
#endif
