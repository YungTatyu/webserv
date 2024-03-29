#ifndef NETWORK_IO_HANDLER_HPP
# define NETWORK_IO_HANDLER_HPP

# include <unistd.h>
# include <sys/types.h>
# include <stdio.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <iostream>
# include <cstdlib>
# include <fcntl.h>
# include "ConfigHandler.hpp"
# include "ConnectionManager.hpp"
# include "SysCallWrapper.hpp"
# include "Server.hpp"

#include <string>

class ConfigHandler;

/* listen socketと結びついたserver config を持つ構造体 */
struct TiedServer
{
	std::vector<const config::Server*>	servers_;
	const std::string address_;
	const unsigned int port_;

	TiedServer( const std::string address, const unsigned int port ) : address_(address), port_(port) {}
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		NetworkIOHandler();
		~NetworkIOHandler();
		int setupSocket( const std::string address, const unsigned int port );
		int receiveRequest( ConnectionManager& connManager, const int cli_sock );
		ssize_t sendResponse( ConnectionManager& connManager, const int cli_sock );
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
