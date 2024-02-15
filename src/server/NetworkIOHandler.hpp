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
# include <poll.h>
# include <fcntl.h>
# include "ServerConfig.hpp"
# include "ConnectionManager.hpp"
# include "SysCallWrapper.hpp"
# include "ActiveEventManager.hpp"


/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ServerConfig *serverConfig );
		int receiveRequest( ConnectionManager& connManager, const int cli_sock );
		ssize_t sendResponse( ConnectionManager& connManager, const int cli_sock );
		void acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager, const int cli_sock );
		int getListenfd();

	private:
		int listenfd_; // リスニングソケットを管理
		static const size_t bufferSize_ = 1024;
};

#endif
