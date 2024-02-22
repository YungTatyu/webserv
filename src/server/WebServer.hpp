#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

# include "ConnectionManager.hpp"
# include "RequestHandler.hpp"
# include "NetworkIOHandler.hpp"
# include "ServerConfig.hpp"
# include "IActiveEventManager.hpp"
# include "PollActiveEventManager.hpp"
# include "IServer.hpp"
# include "PollServer.hpp"
# include "KqueueServer.hpp"
# include "SysCallWrapper.hpp"

class WebServer
{
	public:
		WebServer();
		~WebServer();
		void run();
	private:
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		IActiveEventManager *eventManager;
		IServer *server;
		ServerConfig *serverConfig;
		void initializeServer();
};

#endif
