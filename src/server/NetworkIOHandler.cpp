#include "NetworkIOHandler.hpp"

/* NetworkIOHandlerクラスの実装 */
void NetworkIOHandler::setupSocket( ServerConfig *servConfig )
{
	try
	{
		//creation of the socket
		this->listenfd_ = SysCallWrapper::Socket( AF_INET, SOCK_STREAM, 0 );
		fcntl( this->listenfd_, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

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
		std::exit( EXIT_FAILURE );
	}
}

int NetworkIOHandler::receiveRequest( ConnectionManager& connManager, const int cli_sock )
{
	std::vector<char> buffer( bufferSize_ );
	ssize_t totalBytesRead = 0;

	while ( 1 )
	{
		ssize_t re = recv( cli_sock, buffer.data() + totalBytesRead, bufferSize_, 0 );
		if ( re == 0 && totalBytesRead == 0 ) //クライアントとのコネクションが閉じた時。
		   return 0;
		else if ( re == -1 && totalBytesRead == 0 ) //ソケットが使用不可、またはエラー。
		   return -1;
		else if ( re != bufferSize_ ) //クライアントからのリクエストを読み終えた時。
		   break ;
		totalBytesRead += re;
		buffer.resize( buffer.size() + bufferSize_ );
	}
	connManager.setRawRequest( cli_sock, buffer );
	return 1;
}

int NetworkIOHandler::sendResponse( ConnectionManager &connManager, const int cli_sock )
{
	std::vector<char> response = connManager.getResponse( cli_sock );
	size_t totalSent = 0;
	size_t resSize = response.size();
	const size_t chunkSize = 1024;

	while ( totalSent < resSize )
	{
		size_t currentChunkSize = std::min(chunkSize, resSize - totalSent);
		int sent = send(cli_sock, response.data() + totalSent, currentChunkSize, 0);
		if (sent == -1)
			return -1;
		totalSent += sent;
	}
	return totalSent;
}

void NetworkIOHandler::acceptConnection( ConnectionManager& connManager, EventManager& eventManager )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = SysCallWrapper::Accept( listenfd_, (struct sockaddr *) &cliaddr, &client );
	fcntl( connfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

	struct pollfd setting = EventManager::genPollFd( connfd, POLLIN, 0 );
	connManager.setConnection( connfd );
	eventManager.addEvent( setting );

	// show ip address of newly connected client.
	char clientIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
	std::cout << "> New client connected from IP: " << clientIp << std::endl;
}

void NetworkIOHandler::closeConnection( ConnectionManager& connManager, const int cli_sock )
{
	close( cli_sock );
	connManager.removeConnection( cli_sock );
	printf("%s\n", "< Client disconnected.");
}

int NetworkIOHandler::getListenfd()
{
	return this->listenfd_;
}
