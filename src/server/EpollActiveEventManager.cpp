#include "EpollActiveEventManager.hpp"
#if defined(EPOLL_AVAILABLE)

EpollActiveEventManager::EpollActiveEventManager() {}

EpollActiveEventManager::~EpollActiveEventManager() {}

void	*EpollActiveEventManager::getActiveEvents()
{
	return static_cast<void*>(&(this->active_events_));
}

int	EpollActiveEventManager::getActiveEventsNum()
{
	return this->active_events_num_;
}

void	EpollActiveEventManager:: setActiveEventsNum( const int num )
{
	this->active_events_num_ = num;
}

void	EpollActiveEventManager::addEvent( const void *event )
{
}

void	EpollActiveEventManager::clearAllEvents()
{
}

bool	EpollActiveEventManager::isReadEvent( const void *event)
{
	return true;
}

bool	EpollActiveEventManager::isWriteEvent( const void *event)
{
	return true;
}

bool	EpollActiveEventManager::isErrorEvent( const void *event)
{
	return true;
}

#endif
