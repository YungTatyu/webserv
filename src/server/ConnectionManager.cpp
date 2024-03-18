#include "ConnectionManager.hpp"
#include "HttpResponse.hpp"

std::map<int, std::string> HttpResponse::status_line_map_;
std::map<int, const std::string*> HttpResponse::default_error_page_map_;

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

void ConnectionManager::setFinalResponse( const int fd, const std::vector<char>& final_response )
{
	connections_.at(fd).final_response_ = final_response;
}

const std::vector<char>& ConnectionManager::getFinalResponse( const int fd ) const
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

const std::map<int, ConnectionData> &ConnectionManager::getConnections() const
{
	return this->connections_;
}

void ConnectionManager::setRequest( const int fd, const HttpRequest request )
{
	connections_[fd].request = request;
}

const HttpRequest &ConnectionManager::getRequest( const int fd ) const
{
	return connections_.at(fd).request;
}

void ConnectionManager::setResponse( const int fd, const HttpResponse response )
{
	connections_[fd].response_ = response;
}

HttpRequest &ConnectionManager::getResponse( const int fd )
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

