#ifndef NETWORK_IO_HANDLER_HPP
# define NETWORK_IO_HANDLER_HPP

# include <unistd.h>
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
# include "EventManager.hpp"

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{ public:
		void setupSocket( ServerConfig *serverConfig );
		int receiveRequest( ConnectionManager& connManager, int target );
		void sendResponse( ConnectionManager& connManager, int target );
		void acceptConnection( ConnectionManager& connManager, EventManager& eventManager );
		void closeConnection( ConnectionManager& connManager, int target );
		int getListenfd();

	private:
		int listenfd_; // リスニングソケットを管理
};

#endif
