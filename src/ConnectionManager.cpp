#include "ConnectionManager.hpp"

/* ConnectionManagerクラスの実装 */
void ConnectionManager::addConnection( int connfd )
{
	this->connfd = connfd;
}

int ConnectionManager::getConnection()
{
	return this->connfd;
}

void ConnectionManager::removeConnection()
{
	this->connfd = 0;
}

void ConnectionManager::addContext( const std::vector<char>& context )
{
	this->context = context;
}

const std::vector<char>& ConnectionManager::getContext() const
{
	return this->context;
}


