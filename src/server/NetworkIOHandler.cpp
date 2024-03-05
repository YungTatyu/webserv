#include "NetworkIOHandler.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <utility>

/* NetworkIOHandlerクラスの実装 */
int NetworkIOHandler::setupSocket( const std::string address, const unsigned int port )
{
	try
	{
		//creation of the socket
		const int listen_fd = SysCallWrapper::Socket( AF_INET, SOCK_STREAM, 0 );
		fcntl( this->listenfd_, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

		// socketがtimeout中でもbindできるよう開発中はして、すぐにサーバを再起動できるようにする。
		int yes = 1;
		SysCallWrapper::Setsockopt( this->listenfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) );

		//preparation of the socket address
		struct sockaddr_in servaddr;
		std::memset(&servaddr, 0, sizeof(servaddr));
	
		servaddr.sin_family = AF_INET;
		// TODO: strtoipaddressを適応する
		(void)address;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons( port );

		SysCallWrapper::Bind( this->listenfd_, (struct sockaddr *) &servaddr, sizeof(servaddr) );
		SysCallWrapper::Listen( this->listenfd_, SOMAXCONN );

		std::cout << "Server running on port " << port << std::endl;
		return listen_fd;

	}
	catch ( const std::runtime_error& e )
	{
		std::exit( EXIT_FAILURE );
	}
}

void	NetworkIOHandler::addVServer(const int listen_fd, const TiedServer server)
{
	this->listenfd_map_.insert(std::make_pair(listen_fd, server));
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

ssize_t NetworkIOHandler::sendResponse( ConnectionManager &connManager, const int cli_sock )
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

int NetworkIOHandler::acceptConnection( ConnectionManager& connManager )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = SysCallWrapper::Accept( listenfd_, (struct sockaddr *) &cliaddr, &client );
	fcntl( connfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

	// 新規クライントfdを追加
	connManager.setConnection( connfd );
	connManager.setEvent( connfd, ConnectionData::READ );

	// show ip address of newly connected client.
	char clientIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
	std::cout << "> New client connected from IP: " << clientIp << std::endl;
	return connfd;
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
