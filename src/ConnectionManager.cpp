#include "ConnectionManager.hpp"

/* ConnectionManagerクラスの実装 */
void ConnectionManager::setConnection( int connfd )
{
	this->connfd_ = connfd;
}

int ConnectionManager::getConnection()
{
	return this->connfd_;
}

void ConnectionManager::removeConnection()
{
	this->connfd_ = -1; // 無効なfdを設定しておく
}

void ConnectionManager::setContext( const std::vector<char>& context )
{
	this->context_ = context;
}

const std::vector<char>& ConnectionManager::getContext() const
{
	return this->context_;
}

void ConnectionManager::setResponse( const std::vector<char>& context )
{
	this->response_ = context;
}

const std::vector<char>& ConnectionManager::getResponse() const
{
	return this->response_;
}

