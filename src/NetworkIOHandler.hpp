#ifndef NETWORK_IO_HANDLER_HPP
# define NETWORK_IO_HANDLER_HPP

# include <unistd.h>
# include <stdio.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <iostream>
# include <cstdlib>
# include "ServerConfig.hpp"
# include "ConnectionManager.hpp"
# include "SysCallWrapper.hpp"

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ServerConfig *serverConfig );
		int receiveRequest( ConnectionManager& connManager );
		void sendResponse( ConnectionManager& connManager );
		void acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager );
		int getListenfd();

	private:
		int listenfd_; // リスニングソケットを管理
};

#endif
