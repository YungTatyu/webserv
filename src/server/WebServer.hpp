#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

# include "ConnectionManager.hpp"
# include "RequestHandler.hpp"
# include "NetworkIOHandler.hpp"
# include "ConfigHandler.hpp"
# include "ActiveEventManager.hpp"
# include "SysCallWrapper.hpp"
# include <algorithm>
# include <vector>
# include <map>

class WebServer
{
	public:
		WebServer( const config::Main* config );
		~WebServer();
		void initializeServer();
		void eventLoop();
	private:
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		ActiveEventManager *eventManager;
		ConfigHandler *configHandler;
		std::vector<struct pollfd>	convertToPollfds(const std::map<int, ConnectionData> &connections);
		int	waitForEvents(std::vector<struct pollfd> &pollfds);
		void	addActiveEvents(const std::vector<struct pollfd> &pollfds);
		void	callEventHandler();
};

#endif
