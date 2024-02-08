#ifndef Web_SERVER_HPP
# define Web_SERVER_HPP

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
		int	waitForEvents();
		void	addActiveEvents(const std::vector<struct pollfd> &pollfds);
		void	processEvents(const std::vector<struct pollfd> &pollfds);
		void	callEventHandler(const struct pollfd &pollfd);
};

#endif
