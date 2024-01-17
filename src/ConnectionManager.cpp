#include "ConnectionManager.hpp"
#include <poll.h>

/* ConnectionManagerクラスの実装 */
void ConnectionManager::setConnection( const struct pollfd& pfd )
{
	connections[pfd.fd] = ConnectionData();
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

