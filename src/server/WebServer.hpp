#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

# include "ConnectionManager.hpp"
# include "RequestHandler.hpp"
# include "NetworkIOHandler.hpp"
# include "SysCallWrapper.hpp"
# include "ConfigHandler.hpp"

# include "IActiveEventManager.hpp"
# include "PollActiveEventManager.hpp"
# include "KqueueActiveEventManager.hpp"

# include "IServer.hpp"
# include "PollServer.hpp"
# include "KqueueServer.hpp"

class WebServer
{
	public:
		WebServer( const config::Main* config );
		~WebServer();
		void run();
	private:
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		IActiveEventManager *eventManager;
		IServer *server;
		ConfigHandler *configHandler;
		void initializeServer();
};

#endif
