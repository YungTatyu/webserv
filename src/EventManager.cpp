#include "EventManager.hpp"

void EventManager::updateEvents( int fd, short events )
{
	for ( std::vector<struct pollfd>::iterator cur = fds.begin(); cur != fds.end(); ++cur )
	{
		if ( cur->fd == fd)
		{
			cur->events = events;
		}
	}
}

void EventManager::addPollfd( struct pollfd pfd )
{
       this->fds.push_back( pfd );
}

std::vector<struct pollfd> EventManager::getAllPollfd()
{
       return this->fds;
}

struct pollfd EventManager::getPollfd( int fd )
{
       for ( std::vector<struct pollfd>::iterator cur = fds.begin(); cur != fds.end(); ++cur )
       {
               if ( cur->fd == fd )
               {
                       return *cur;
               }
       }

       // if not matching fd is found, return invalid fd.
       return ( genPollFd( -1, 0, 0 ) );
}

struct pollfd EventManager::genPollFd( int fd, short events, short revents )
{
       struct pollfd tmp;
       tmp.fd = fd;
       tmp.events = events;
       tmp.revents = revents;
       return tmp;
}
