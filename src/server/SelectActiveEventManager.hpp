#ifndef SELECT_ACTIVE_EVENT_MANAGER_HPP
#define SELECT_ACTIVE_EVENT_MANAGER_HPP

#include <sys/select.h>

#include <vector>

#include "IActiveEventManager.hpp"

struct SelectEvent {
 private:
  SelectEvent();

 public:
  enum SELECT_EVENT {
    SELECT_READ,
    SELECT_WRITE
  };
  int fd_;
  SELECT_EVENT event_;

  SelectEvent(int fd, SELECT_EVENT event) : fd_(fd), event_(event) {}
  SelectEvent(const SelectEvent &other) { *this = other; }
  SelectEvent &operator=(const SelectEvent &other) {
    if (this != &other) {
      this->fd_ = other.fd_;
      this->event_ = other.event_;
    }
    return *this;
  }
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
