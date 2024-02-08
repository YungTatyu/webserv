#include "ActiveEventManager.hpp"

void ActiveEventManager::updateEvents( const int fd, const short events )
{
	for ( std::vector<struct pollfd>::iterator cur = active_events_.begin(); cur != active_events_.end(); ++cur )
	{
		if ( cur->fd == fd)
		{
			cur->events = events;
		}
	}
}

void ActiveEventManager::addEvent( const struct pollfd pfd )
{
	this->active_events_.push_back( pfd );
}

void ActiveEventManager::clearAllEvents()
{
	this->active_events_.clear();
}

const std::vector<struct pollfd>& ActiveEventManager::getAllPollfd() const
{
	return this->active_events_;
}

struct pollfd ActiveEventManager::getPollfd( const int fd ) const
{
	for ( std::vector<struct pollfd>::const_iterator cur = active_events_.begin(); cur != active_events_.end(); ++cur )
	{
	       if ( cur->fd == fd )
	       {
		       return *cur;
	       }
	}

	// if no matching fd is found, return invalid fd.
	return ( genPollFd( -1, 0, 0 ) );
}

struct pollfd ActiveEventManager::genPollFd( const int fd, const short events, const short revents )
{
	struct pollfd tmp;
	tmp.fd = fd;
	tmp.events = events;
	tmp.revents = revents;
	return tmp;
}

bool ActiveEventManager::isInvalidFd( const struct pollfd& pfd )
{
	return pfd.fd == -1;
}
