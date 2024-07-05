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
  KqueueActiveEventManager(const KqueueActiveEventManager &);
  ~KqueueActiveEventManager();
  KqueueActiveEventManager &operator=(const KqueueActiveEventManager &);
  void *getActiveEvents();
  int getActiveEventsNum();
  void setActiveEventsNum(int num);
  void addEvent(const void *event);
  void clearAllEvents();
  void reallocActiveEvents(std::size_t size);
  bool isReadEvent(const void *event);
  bool isWriteEvent(const void *event);
  bool isErrorEvent(const void *event);
  bool isEofEvent(const void *event);

 private:
  std::vector<struct kevent> active_events_;
  int active_events_num_;
};

#endif
#endif
