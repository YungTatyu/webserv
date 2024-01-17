#include "NetworkIOHandler.hpp"

/* NetworkIOHandlerクラスの実装 */
void NetworkIOHandler::setupSocket( ServerConfig *servConfig )
{
	try
	{
		//creation of the socket
		this->listenfd_ = SysCallWrapper::Socket( AF_INET, SOCK_STREAM, 0 );

		// socketがtimeout中でもbindできるよう開発中はして、すぐにサーバを再起動できるようにする。
		int yes = 1;
		SysCallWrapper::Setsockopt( this->listenfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) );

		//preparation of the socket address
		struct sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons( servConfig->getServPort() );
		
		SysCallWrapper::Bind( this->listenfd_, (struct sockaddr *) &servaddr, sizeof(servaddr) );
		SysCallWrapper::Listen( this->listenfd_, servConfig->getListenQ() );
		
		std::cout << "Server running on port " << servConfig->getServPort() << std::endl;

	}
	catch ( const std::runtime_error& e )
	{
		// std::cout << e.what() << std::endl;
		if ( this->listenfd_ != -1 )
			close( this->listenfd_ );
		exit( EXIT_FAILURE );
	}
}

int NetworkIOHandler::receiveRequest( ConnectionManager& connManager, int target )
{
	// char *buf[MAXLINE];
	std::vector<char> buffer(1024);
	if ( recv( target, buffer.data(), buffer.size(), 0 ) <= 0 )
		return -1;
	connManager.setContext( target, buffer );
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
	connfd = SysCallWrapper::Accept( listenfd_, (struct sockaddr *) &cliaddr, &client );

	struct pollfd setting;
	setting.fd = connfd;
	setting.events = POLLIN;
	setting.revents = 0;
	connManager.setConnection( setting );

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
	return this->listenfd_;
}

