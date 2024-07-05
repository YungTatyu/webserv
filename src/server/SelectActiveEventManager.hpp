#ifndef SELECT_ACTIVE_EVENT_MANAGER_HPP
#define SELECT_ACTIVE_EVENT_MANAGER_HPP

#include <sys/select.h>

#include <vector>

#include "IActiveEventManager.hpp"

struct SelectEvent {
  enum SELECT_EVENT {
    SELECT_READ,
    SELECT_WRITE
  };
  int fd_;
  SELECT_EVENT event_;
  SelectEvent(int fd, SELECT_EVENT event) : fd_(fd), event_(event) {}
  SelectEvent() : fd_(-1), event_(SELECT_READ) {}
};

class SelectActiveEventManager : public IActiveEventManager {
 public:
  SelectActiveEventManager();
  SelectActiveEventManager(const SelectActiveEventManager &other);
  ~SelectActiveEventManager();
  SelectActiveEventManager &operator=(const SelectActiveEventManager &other);
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
  std::vector<SelectEvent> active_events_;
  int active_events_num_;
};

#endif
