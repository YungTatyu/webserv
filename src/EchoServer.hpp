#ifndef ECHO_SERVER_HPP
# define ECHO_SERVER_HPP

# include "ConnectionManager.hpp"
# include "RequestHandler.hpp"
# include "NetworkIOHandler.hpp"
# include "ServerConfig.hpp"

class EchoServer
{
	public:
		EchoServer();
		~EchoServer();
		void initializeServer();
		void eventLoop();
	
	private:
		int listenfd; // listenソケットのfd
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		ServerConfig *serverConfig;
};

#endif
