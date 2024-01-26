#include "EventManager.hpp"

void EventManager::updateEvents( const int fd, const short events )
{
	for ( std::vector<struct pollfd>::iterator cur = fds.begin(); cur != fds.end(); ++cur )
	{
		if ( cur->fd == fd)
		{
			cur->events = events;
		}
	}
}

void EventManager::addEvent( const struct pollfd pfd )
{
       this->fds.push_back( pfd );
}

std::vector<struct pollfd> EventManager::getAllPollfd() const
{
       return this->fds;
}

struct pollfd EventManager::getPollfd( const int fd ) const
{
       for ( std::vector<struct pollfd>::const_iterator cur = fds.begin(); cur != fds.end(); ++cur )
       {
               if ( cur->fd == fd )
               {
                       return *cur;
               }
       }

       // if no matching fd is found, return invalid fd.
       return ( genPollFd( -1, 0, 0 ) );
}

struct pollfd EventManager::genPollFd( const int fd, const short events, const short revents )
{
       struct pollfd tmp;
       tmp.fd = fd;
       tmp.events = events;
       tmp.revents = revents;
       return tmp;
}

bool EventManager::isInvalidFd( const struct pollfd& pfd )
{
	return pfd.fd == -1;
}

