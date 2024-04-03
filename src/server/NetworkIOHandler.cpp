#include "NetworkIOHandler.hpp"
#include "ConnectionManager.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <utility>
#include <algorithm>

NetworkIOHandler::NetworkIOHandler() {}

NetworkIOHandler::~NetworkIOHandler()
{
	closeAllListenSockets(); // リスニングソケットのクローズ
}

/* NetworkIOHandlerクラスの実装 */
int NetworkIOHandler::setupSocket( const std::string address, const unsigned int port )
{
	try
	{
		//creation of the socket
		const int listen_fd = SysCallWrapper::Socket( AF_INET, SOCK_STREAM, 0 );
		fcntl( listen_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

		// socketがtimeout中でもbindできるよう開発中はして、すぐにサーバを再起動できるようにする。
		int yes = 1;
		SysCallWrapper::Setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) );

		//preparation of the socket address
		struct sockaddr_in servaddr;
		std::memset(&servaddr, 0, sizeof(servaddr));
	
		servaddr.sin_family = AF_INET;
		// TODO: strtoipaddressを適応する
		(void)address;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons( port );

		// 失敗したとき？
		SysCallWrapper::Bind( listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr) );
		SysCallWrapper::Listen( listen_fd, SOMAXCONN );

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
	std::vector<unsigned char> buffer( bufferSize_ );
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

ssize_t NetworkIOHandler::receiveCgiResponse( ConnectionManager& connManager, const int sock )
{
	const static size_t buffer_size = 1024;
	std::vector<unsigned char>	buffer(buffer_size);

	ssize_t re = recv(sock, buffer.data(), buffer_size, 0);
	if (re > 0)
		connManager.addCgiResponse(sock, buffer);
	return re;
}

ssize_t NetworkIOHandler::sendResponse( ConnectionManager &connManager, const int cli_sock )
{
	std::vector<unsigned char> response = connManager.getFinalResponse( cli_sock );
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

ssize_t NetworkIOHandler::sendRequestBody( ConnectionManager& connManager, const int sock )
{
	const static size_t buffer_size = 1024;
	const std::string	body = connManager.getRequest(sock).body;
	const size_t	sent_bytes = connManager.getSentBytes(sock);
	const size_t	rest = body.size() - sent_bytes;

	ssize_t	re = send(sock, &body.c_str()[sent_bytes], std::min(buffer_size, rest), 0);
	if (re > 0)
		connManager.addSentBytes(sock, re);
	return re;
}

int NetworkIOHandler::acceptConnection( ConnectionManager& connManager, const int listen_fd )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = SysCallWrapper::Accept( listen_fd, (struct sockaddr *) &cliaddr, &client );
	if (connfd == -1)
		return connfd;
	fcntl( connfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

	// 新規クライントfdを追加
	connManager.setConnection( connfd );
	connManager.setEvent( connfd, ConnectionData::EV_READ );
	connManager.setTiedServer( connfd, &this->listenfd_map_[listen_fd]);

	// show ip address of newly connected client.
	char clientIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
	std::cout << "> New client connected from IP: " << clientIp << std::endl;
	return connfd;
}

bool	NetworkIOHandler::isListenSocket(const int listen_fd) const
{
	try
	{
		this->listenfd_map_.at(listen_fd);
		return true;
	}
	catch(const std::out_of_range& e)
	{
		return false;
	}
}

void NetworkIOHandler::closeConnection( ConnectionManager& connManager, const int cli_sock )
{
	close( cli_sock );
	bool	cgi = connManager.isCgiSocket(cli_sock);
	if (cgi)
		connManager.resetCgiSockets(cli_sock);
	connManager.removeConnection( cli_sock, cgi );
	std::cerr << "client disconnected\n";
}

const std::map<int, TiedServer>&	NetworkIOHandler::getListenfdMap()
{
	return this->listenfd_map_;
}

void	NetworkIOHandler::closeAllListenSockets()
{
	for (std::map<int, TiedServer>::iterator it = this->listenfd_map_.begin();
		it != this->listenfd_map_.end();
		++it
	)
	{
		close(it->first);
	}
	this->listenfd_map_.clear();
}
