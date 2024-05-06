#include "EpollActiveEventManager.hpp"

#if defined(EPOLL_AVAILABLE)
#include <sys/epoll.h>

EpollActiveEventManager::EpollActiveEventManager() {}

EpollActiveEventManager::~EpollActiveEventManager() {}

void *EpollActiveEventManager::getActiveEvents() { return static_cast<void *>(&(this->active_events_)); }

int EpollActiveEventManager::getActiveEventsNum() { return this->active_events_num_; }

void EpollActiveEventManager::setActiveEventsNum(const int num) { this->active_events_num_ = num; }

void EpollActiveEventManager::addEvent(const void *event) { (void)event; }

void EpollActiveEventManager::clearAllEvents() { this->active_events_.clear(); }

bool EpollActiveEventManager::isReadEvent(const void *event, const bool is_cgi_sock) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLIN) && !isErrorEvent(event) && !isEofEvent(event, is_cgi_sock);
}

bool EpollActiveEventManager::isWriteEvent(const void *event, const bool is_cgi_sock) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLOUT) && !isErrorEvent(event) && !isEofEvent(event, is_cgi_sock);
}

bool	EpollActiveEventManager::isEofEvent( const void *event, const bool is_cgi_sock)
{
	const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
	if (is_cgi_sock) {
		return false;
	}
	return (ep_event->events & EPOLLHUP) && !isErrorEvent(event);
}

bool EpollActiveEventManager::isErrorEvent(const void *event) {
  const struct epoll_event *ep_event = static_cast<const struct epoll_event *>(event);
  return (ep_event->events & EPOLLERR);
}

#endif
