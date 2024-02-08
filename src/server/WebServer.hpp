#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

# include "ConnectionManager.hpp"
# include "RequestHandler.hpp"
# include "NetworkIOHandler.hpp"
# include "ServerConfig.hpp"
# include "ActiveEventManager.hpp"
# include "SysCallWrapper.hpp"
# include <algorithm>
# include <vector>
# include <map>

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
		ActiveEventManager *eventManager;
		ServerConfig *serverConfig;
		std::vector<struct pollfd>	convertToPollfds(const std::map<int, ConnectionData> &connections);
		int	waitForEvents(std::vector<struct pollfd> &pollfds);
		void	addActiveEvents(const std::vector<struct pollfd> &pollfds);
		void	callEventHandler(const std::vector<struct pollfd> &pollfds);
};

#endif
