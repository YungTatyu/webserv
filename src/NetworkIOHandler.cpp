#include "NetworkIOHandler.hpp"

/* NetworkIOHandlerクラスの実装 */
void NetworkIOHandler::setupSocket( ServerConfig *servConfig )
{
	struct sockaddr_in servaddr;

	//creation of the socket
	this->listenfd = socket (AF_INET, SOCK_STREAM, 0);

	// socketがtimeout中でもbindできるよう開発中はにして、すぐにサーバを再起動できるようにする。
	int yes = 1;
	if (setsockopt(this->listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		perror("setsockopt");
	}

	//preparation of the socket address
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons( servConfig->getServPort() );
	
	bind (this->listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen (this->listenfd, servConfig->getListenQ());
	
	std::cout << "Server running on port " << servConfig->getServPort() << std::endl;
}

int NetworkIOHandler::receiveRequest( ConnectionManager& connManager, int target )
{
	// char *buf[MAXLINE];
	std::vector<char> buffer(1024);
	if ( recv( target, buffer.data(), buffer.size(), 0 ) <= 0 )
		return -1;
	connManager.addContext( target, buffer );
	return 0;
}

void NetworkIOHandler::sendResponse( ConnectionManager &connManager, int target )
{	
	send( target, connManager.getResponse( target ).data(), connManager.getResponse( target ).size(), 0);
}

void NetworkIOHandler::acceptConnection( ConnectionManager& connManager )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);

	struct pollfd setting;
	setting.fd = connfd;
	setting.events = POLLIN;
	setting.revents = 0;
	connManager.addConnection( setting );

	// show ip address of newly connected client.
	char clientIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
	std::cout << "> New client connected from IP: " << clientIp << std::endl;
}

void NetworkIOHandler::closeConnection( ConnectionManager& connManager, int target )
{
	close( target );
	connManager.removeConnection( target );
	printf("%s\n", "< Client disconnected.");
}

int NetworkIOHandler::getListenfd()
{
	return this->listenfd;
}

