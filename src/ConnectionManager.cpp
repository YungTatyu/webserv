#include "ConnectionManager.hpp"
#include <poll.h>

/* ConnectionManagerクラスの実装 */
void ConnectionManager::setConnection( const struct pollfd& pfd )
{
	connections_[pfd.fd] = ConnectionData();
}

void ConnectionManager::removeConnection( const int fd )
{
	connections_.erase( fd );
}

void ConnectionManager::setContext( const int fd, const std::vector<char>& context )
{
	connections_[fd].context = context;
}


const std::vector<char>& ConnectionManager::getContext( int fd ) const
{
	return connections_.at(fd).context;
}

void ConnectionManager::setResponse( int fd, const std::vector<char>& response )
{
	connections_.at(fd).response = response;
}

const std::vector<char>& ConnectionManager::getResponse( int fd ) const
{
	return connections_.at(fd).response;
}

