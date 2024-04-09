#ifndef NETWORK_IO_HANDLER_HPP
# define NETWORK_IO_HANDLER_HPP

# include <unistd.h>
# include <sys/types.h>
# include <stdio.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <map>
# include <iostream>
# include <cstdlib>
# include <fcntl.h>
# include "SysCallWrapper.hpp"
# include "Server.hpp"
# include "Listen.hpp"

#include <string>

class ConfigHandler;
class ConnectionManager;

/* listen socketと結びついたserver config を持つ構造体 */
struct TiedServer
{
	std::vector<const config::Server*>	servers_;
	const std::string addr_;
	const unsigned int port_;

	TiedServer() : addr_(config::Listen::kDefaultAddress_), port_(config::Listen::kDefaultPort_) {};
	TiedServer( const std::string addr, const unsigned int port ) : addr_(addr), port_(port) {}

	TiedServer&	operator=( const TiedServer& other )
	{
		if (this != &other)
		{
			this->servers_ = other.servers_;
			//this->addr_ = other.addr_;
			//this->port_ = other.port_;
		}
		return *this;
	}
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		NetworkIOHandler();
		~NetworkIOHandler();
		int	setupSocket( const std::string address, const unsigned int port );
		int receiveRequest( ConnectionManager& connManager, const int cli_sock );
		int sendResponse( ConnectionManager& connManager, const int cli_sock );
		ssize_t receiveCgiResponse( ConnectionManager& connManager, const int sock );
		ssize_t sendRequestBody( ConnectionManager& connManager, const int sock );
		int acceptConnection( ConnectionManager& connManager, const int listen_fd );
		void closeConnection( ConnectionManager& connManager, const int cli_sock );
		void	closeAllListenSockets();
		const std::map<int, TiedServer>& getListenfdMap();
		void	addVServer(const int listen_fd, const TiedServer server);
		bool	isListenSocket(const int listen_fd) const;

	private:
		std::map<int, TiedServer> listenfd_map_; // リスニングソケットとそれに紐づくserver configを管理
		static const size_t bufferSize_ = 1024;
};

#endif
