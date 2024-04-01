#include "ConnectionManager.hpp"
#include <unistd.h>

ConnectionManager::ConnectionManager() {}

ConnectionManager::~ConnectionManager()
{
	closeAllConnections();
}

/* ConnectionManagerクラスの実装 */
void ConnectionManager::setConnection( const int fd )
{
	ConnectionData	*ptr = new ConnectionData();
	connections_[fd] = *ptr;
}

/**
 * @brief connection mapから削除
 * cgiの場合は、connection dataを削除しない
 * clientがデータを必要とするため
 * 
 * @param fd 
 * @param cgi 
 */
void ConnectionManager::removeConnection( const int fd, const bool cgi )
{
	if (!cgi)
		delete &(connections_.at(fd));
	connections_.erase( fd );
}

void ConnectionManager::setRawRequest( const int fd, const std::vector<unsigned char>& rawRequest )
{
	connections_[fd].rawRequest = rawRequest;
}

const std::vector<unsigned char>& ConnectionManager::getRawRequest( const int fd ) const
{
	return connections_.at(fd).rawRequest;
}

void ConnectionManager::setFinalResponse( const int fd, const std::vector<unsigned char>& final_response )
{
	connections_.at(fd).final_response_ = final_response;
}

const std::vector<unsigned char>& ConnectionManager::getFinalResponse( const int fd ) const
{
	return connections_.at(fd).final_response_;
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

const std::map<int, ConnectionData&> &ConnectionManager::getConnections() const
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

void ConnectionManager::setResponse( const int fd, const HttpResponse response )
{
	connections_[fd].response_ = response;
}

HttpResponse &ConnectionManager::getResponse( const int fd )
{
	return connections_.at(fd).response_;
}

void	ConnectionManager::setTiedServer( const int fd, const TiedServer* tied_server )
{
	connections_[fd].tied_server_ = tied_server;
}

const TiedServer&	ConnectionManager::getTiedServer( const int fd ) const
{
	return *connections_.at(fd).tied_server_;
}

void	ConnectionManager::closeAllConnections()
{
	for (std::map<int, ConnectionData&>::iterator it = this->connections_.begin();
		it != this->connections_.end();
		++it
	)
	{
		close(it->first);
		delete &(it->second);
	}
	this->connections_.clear();
}
