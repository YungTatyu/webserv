#include "EpollActiveEventManager.hpp"

#if defined(EPOLL_AVAILABLE)
#include <sys/epoll.h>

EpollActiveEventManager::EpollActiveEventManager() {}

EpollActiveEventManager::EpollActiveEventManager(const EpollActiveEventManager &other) { *this = other; }

EpollActiveEventManager::~EpollActiveEventManager() {}

EpollActiveEventManager &EpollActiveEventManager::operator=(const EpollActiveEventManager &other) {
  if (this != &other) {
    this->active_events_ = other.active_events_;
    this->active_events_num_ = other.active_events_num_;
  }
  return *this;
}

void *EpollActiveEventManager::getActiveEvents() { return static_cast<void *>(&(this->active_events_)); }

int EpollActiveEventManager::getActiveEventsNum() { return this->active_events_num_; }

void EpollActiveEventManager::setActiveEventsNum(int num) { this->active_events_num_ = num; }

void EpollActiveEventManager::addEvent(const void *event) { (void)event; }

void EpollActiveEventManager::clearAllEvents() { this->active_events_.clear(); }

bool EpollActiveEventManager::isReadEvent(const void *event) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLIN) && !isErrorEvent(event) && !isEofEvent(event);
}

bool EpollActiveEventManager::isWriteEvent(const void *event) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLOUT) && !isErrorEvent(event) && !isEofEvent(event);
}

/**
 * @brief eof
 *
 * eofイベントが発生する際、エラーイベントも発生しうるので、エラーイベントはみない
 *
 * @param event
 * @return true
 * @return false
 */
bool EpollActiveEventManager::isEofEvent(const void *event) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLHUP);
}

bool EpollActiveEventManager::isErrorEvent(const void *event) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLERR);
}

#endif
