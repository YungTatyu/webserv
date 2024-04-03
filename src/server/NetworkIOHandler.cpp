#include "NetworkIOHandler.hpp"
#include "ConnectionManager.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <utility>

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

       ssize_t re = recv( cli_sock, buffer.data(), bufferSize_, 0 );
       if ( re == 0 ) //クライアントとのコネクションが閉じた時。
               return 0;
       else if ( re == -1 ) //ソケットが使用不可、またはエラー。
               return -1;

       connManager.addRawRequest( cli_sock, buffer );

       if ( re == bufferSize_ ) // bufferSize_分だけ読んだ時。次のループで残りを読む。 ちょうどrecvでbuffersize分読んだ時はどうなる？？（次readイベント発生し 可能性）
               return 2;

	return 1;
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

int NetworkIOHandler::acceptConnection( ConnectionManager& connManager, const int listen_fd )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = SysCallWrapper::Accept( listen_fd, (struct sockaddr *) &cliaddr, &client );
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
	connManager.removeConnection( cli_sock );
	printf("%s\n", "< Client disconnected.");
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
