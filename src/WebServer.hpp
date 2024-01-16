#ifndef Web_SERVER_HPP
# define Web_SERVER_HPP

# include "ConnectionManager.hpp"
# include "RequestHandler.hpp"
# include "NetworkIOHandler.hpp"
# include "ServerConfig.hpp"

class WebServer
{
	public:
		WebServer();
		~WebServer();
		void initializeServer();
		void eventLoop();
	
	private:
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		ServerConfig *serverConfig;
};

#endif
