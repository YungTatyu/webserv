#include "NetworkIOHandler.hpp"

/* NetworkIOHandlerクラスの実装 */
void NetworkIOHandler::setupSocket( ServerConfig *servConfig )
{
	struct sockaddr_in servaddr;

	//creation of the socket
	this->listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
	//preparation of the socket address
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons( servConfig->getServPort() );
	
	bind (this->listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen (this->listenfd, servConfig->getListenQ());
	
	printf("%s\n","Server running...waiting for connections.");
}

void NetworkIOHandler::receiveData( ConnectionManager& connManager )
{
	// char *buf[MAXLINE];
	std::vector<char> buffer(1024);
	recv( connManager.getConnection(), buffer.data(), buffer.size(), 0 );
	connManager.addContext( buffer );
}

void NetworkIOHandler::sendData( ConnectionManager &connManager )
{	
	send(connManager.getConnection(), connManager.getContext().data(), connManager.getContext().size(), 0);
}

void NetworkIOHandler::acceptConnection( ConnectionManager& connManager )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);
	connManager.addConnection( connfd );
	printf("%s\n","Received request...");
}

void NetworkIOHandler::closeConnection( ConnectionManager& connManager )
{
	close( connManager.getConnection() );
	connManager.removeConnection();
}


