#include "ConnectionManager.hpp"
#include <poll.h>

/* ConnectionManagerクラスの実装 */
void ConnectionManager::setConnection( const struct pollfd& pfd )
{
	fds.push_back( pfd );
	connections[pfd.fd] = ConnectionData();
}

void ConnectionManager::updateEvents( int fd, short revents )
{
	for ( std::vector<struct pollfd>::iterator cur = fds.begin(); cur != fds.end(); ++cur )
	{
		if ( cur->fd == fd)
		{
			if ( revents == POLLIN )
				cur->events = POLLIN;
			else if ( revents == POLLOUT )
				cur->events = POLLOUT;
		}
	}
}

void ConnectionManager::removeConnection( int fd )
{
	connections.erase( fd );
}

void ConnectionManager::setContext( int fd, const std::vector<char>& context )
{
	connections[fd].context = context;
}


const std::vector<char>& ConnectionManager::getContext( int fd ) const
{
	return connections.at(fd).context;
}

void ConnectionManager::setResponse( int fd, const std::vector<char>& response )
{
	connections.at(fd).response = response;
}

const std::vector<char>& ConnectionManager::getResponse( int fd ) const
{
	return connections.at(fd).response;
}

