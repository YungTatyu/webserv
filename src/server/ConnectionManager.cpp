#include "ConnectionManager.hpp"

/* ConnectionManagerクラスの実装 */
void ConnectionManager::setConnection( const int fd )
{
	connections_[fd] = ConnectionData();
}

void ConnectionManager::removeConnection( const int fd )
{
	connections_.erase( fd );
}

void ConnectionManager::setRawRequest( const int fd, const std::vector<char>& rawRequest )
{
	connections_[fd].rawRequest = rawRequest;
}


const std::vector<char>& ConnectionManager::getRawRequest( const int fd ) const
{
	return connections_.at(fd).rawRequest;
}

void ConnectionManager::setResponse( const int fd, const std::vector<char>& response )
{
	connections_.at(fd).response = response;
}

const std::vector<char>& ConnectionManager::getResponse( const int fd ) const
{
	return connections_.at(fd).response;
}

/**
* 
* 監視するイベントを更新
* イベントをupdateする際にも使用
* 
*/
void ConnectionManager::setEvent( const int fd, const ConnectionData::EVENT event )
{
	connections_[fd].event = event;
}

ConnectionData::EVENT ConnectionManager::getEvent( const int fd ) const
{
	return connections_.at(fd).event;
}

const std::map<int, ConnectionData> &ConnectionManager::getConnections() const
{
	return this->connections_;
}

void ConnectionManager::setRequest( const int fd, const HttpRequest request )
{
	connections_[fd].request = request;
}

HttpRequest &ConnectionManager::getRequest( const int fd )
{
	return connections_.at(fd).request;
}
