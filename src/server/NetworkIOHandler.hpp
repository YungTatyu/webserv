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

class ConfigHandler;

/* listen socketと結びついたserver config を持つ構造体 */
struct TiedServer
{
	std::vector<config::Server*>	servers_;
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ConfigHandler *configHandler );
		int receiveRequest( ConnectionManager& connManager, const int cli_sock );
		ssize_t sendResponse( ConnectionManager& connManager, const int cli_sock );
		void acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager, const int cli_sock );
		int getListenfd();

	private:
		int listenfd_; // リスニングソケットを管理
		std::map<int, struct TiedServer> listenfd_map_; // リスニングソケットとそれに紐づくserver configを管理
		static const size_t bufferSize_ = 1024;
};

#endif
