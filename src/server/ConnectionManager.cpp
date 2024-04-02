#include "ConnectionManager.hpp"
#include <unistd.h>
#include <algorithm>

ConnectionManager::ConnectionManager() {}

ConnectionManager::~ConnectionManager()
{
	closeAllConnections();
}

/**
 * @brief client or listen socketを登録する
 * 
 * @param fd 
 */
void ConnectionManager::setConnection( const int fd )
{
	connections_[fd] = new ConnectionData();
}

/**
 * @brief cgiのsocketを登録する
 * 
 * @param cli_sock cgiのsocketに紐づいているクライアントのソケット
 * @param event 
 */
void ConnectionManager::setCgiConnection( const int cli_sock, const ConnectionData::EVENT event )
{
	ConnectionData	*cd = this->connections_.at(cli_sock); 
	const int	cgi_sock = cd->cgi_handler_.sockets_[cgi::SOCKET_PARENT];
	this->connections_.insert(std::make_pair(cgi_sock, cd));
	// cgi のイベントに更新
	this->connections_.at(cli_sock)->event = event;
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
	connections_[fd]->rawRequest = rawRequest;
}

const std::vector<unsigned char>& ConnectionManager::getRawRequest( const int fd ) const
{
	return connections_.at(fd)->rawRequest;
}

void ConnectionManager::setFinalResponse( const int fd, const std::vector<unsigned char>& final_response )
{
	connections_.at(fd)->final_response_ = final_response;
}

const std::vector<unsigned char>& ConnectionManager::getFinalResponse( const int fd ) const
{
	return connections_.at(fd)->final_response_;
}

/**
* 
* 監視するイベントを更新
* イベントをupdateする際にも使用
* 
*/
void ConnectionManager::setEvent( const int fd, const ConnectionData::EVENT event )
{
	connections_[fd]->event = event;
}

ConnectionData::EVENT ConnectionManager::getEvent( const int fd ) const
{
	return connections_.at(fd)->event;
}

const std::map<int, ConnectionData*> &ConnectionManager::getConnections() const
{
	return this->connections_;
}

void ConnectionManager::setRequest( const int fd, const HttpRequest request )
{
	connections_[fd]->request = request;
}

HttpRequest &ConnectionManager::getRequest( const int fd )
{
	return connections_.at(fd)->request;
}

void ConnectionManager::setResponse( const int fd, const HttpResponse response )
{
	connections_[fd]->response_ = response;
}

HttpResponse &ConnectionManager::getResponse( const int fd )
{
	return connections_.at(fd)->response_;
}

void	ConnectionManager::addCgiResponse( const int fd, const std::vector<unsigned char>& v )
{
	std::vector<unsigned char>&	cgi_response = this->connections_.at(fd)->cgi_response_;
	cgi_response.resize(cgi_response.size() + v.size());
	std::copy(v.begin(), v.end(), std::back_inserter(cgi_response));
}

const std::vector<unsigned char>&	ConnectionManager::getCgiResponse( const int fd ) const
{
	return this->connections_.at(fd)->cgi_response_;
}

void	ConnectionManager::setTiedServer( const int fd, const TiedServer* tied_server )
{
	connections_[fd]->tied_server_ = tied_server;
}

const TiedServer&	ConnectionManager::getTiedServer( const int fd ) const
{
	return *(connections_.at(fd)->tied_server_);
}

const cgi::CGIHandler& ConnectionManager::getCgiHandler( const int fd ) const
{
	return connections_.at(fd)->cgi_handler_;
}

size_t	ConnectionManager::getSentBytes( const int fd ) const
{
	return connections_.at(fd)->sent_bytes_;
}

void	ConnectionManager::addSentBytes( const int fd, size_t bytes )
{
	connections_.at(fd)->sent_bytes_ += bytes;
}

void	ConnectionManager::resetSentBytes( const int fd )
{
	connections_.at(fd)->sent_bytes_ = 0;
}


void	ConnectionManager::closeAllConnections()
{
	for (std::map<int, ConnectionData*>::iterator it = this->connections_.begin();
		it != this->connections_.end();
		++it
	)
	{
		close(it->first);
		delete &(it->second);
	}
	this->connections_.clear();
}

bool	ConnectionManager::isCgiSocket( const int fd ) const
{
	const int	cgi_sock = this->connections_.at(fd)->cgi_handler_.sockets_[cgi::SOCKET_PARENT];
	return fd == cgi_sock;
}
