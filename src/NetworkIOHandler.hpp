#ifndef NETWORK_IO_HANDLER_HPP
# define NETWORK_IO_HANDLER_HPP

# include <unistd.h>
# include <stdio.h>
# include <vector>
# include <netinet/in.h>
# include "ServerConfig.hpp"
# include "ConnectionManager.hpp"

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ServerConfig *serverConfig );
		int receiveRequest( ConnectionManager& connManager );
		void sendResponse( ConnectionManager& connManager );
		void acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager );

	private:
		int listenfd;
};

#endif
