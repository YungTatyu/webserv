#include "ConnectionManager.hpp"
#include <poll.h>

/* ConnectionManagerクラスの実装 */
void ConnectionManager::addConnection( const struct pollfd& pfd )
{
	fds.push_back( pfd );
	connections[pfd.fd] = ConnectionData();
}

void ConnectionManager::updateEvents( int fd, short revents )
{
	(void)revents;
	/*
	if ( events == POLLIN )
		connections[fd].pollfd.events = POLLOUT;
	else if ( events == POLLOUT )
		connections[fd].pollfd.events = POLLIN;
	*/
	for ( std::vector<struct pollfd>::iterator cur = fds.begin(); cur != fds.end(); ++cur )
	{
		if ( cur->fd == fd )
		{
			cur->events = POLLIN;
		}
	}
}

void ConnectionManager::removeConnection( int fd )
{
	connections.erase( fd );
}

void ConnectionManager::addContext( int fd, const std::vector<char>& context )
{
	connections[fd].context = context;
}


const std::vector<char>& ConnectionManager::getContext( int fd ) const
{
	return connections.at(fd).context;
}

void ConnectionManager::addResponse( int fd, const std::vector<char>& response )
{
	connections.at(fd).response = response;
}

const std::vector<char>& ConnectionManager::getResponse( int fd ) const
{
	return connections.at(fd).response;
}

