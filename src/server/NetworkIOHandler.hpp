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

class ConfigHandler;
class ConnectionManager;

/* listen socketと結びついたserver config を持つ構造体 */
struct TiedServer
{
	std::vector<const config::Server*>	servers_;
	const std::string addr_;
	const unsigned int port_;

	TiedServer() : addr_("127.0.0.1"), port_(80) {};
	TiedServer( const std::string addr, const unsigned int port ) : addr_(addr), port_(port) {}
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ConfigHandler *configHandler );
		int receiveRequest( ConnectionManager& connManager, const int cli_sock );
		ssize_t sendResponse( ConnectionManager& connManager, const int cli_sock );
		int acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager, const int cli_sock );
		int getListenfd();

	private:
		int listenfd_; // リスニングソケットを管理
		std::map<int, struct TiedServer> listenfd_map_; // リスニングソケットとそれに紐づくserver configを管理
		static const size_t bufferSize_ = 1024;
};

#endif
