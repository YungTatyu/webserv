#ifndef POLL_ACTIVE_EVENT_MANAGER_HPP
#define POLL_ACTIVE_EVENT_MANAGER_HPP

#include <vector>

#include "IActiveEventManager.hpp"

class PollActiveEventManager : public IActiveEventManager {
 public:
  PollActiveEventManager();
  ~PollActiveEventManager();
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
  PollActiveEventManager(const PollActiveEventManager &);
  PollActiveEventManager &operator=(const PollActiveEventManager &);

  std::vector<struct pollfd> active_events_;
  int active_events_num_;
};

#endif
