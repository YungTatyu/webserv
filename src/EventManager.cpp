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
