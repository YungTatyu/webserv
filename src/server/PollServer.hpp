#ifndef POLL_SERVER_HPP
#define POLL_SERVER_HPP

#include <vector>
#include <map>

#include "IServer.hpp"

class PollServer : public IServer
{
	public:
		PollServer();
		~PollServer();
		void	eventLoop(ConnectionManager* conn_manager,
								IActiveEventManager* event_manager,
								NetworkIOHandler* io_handler,
								RequestHandler* request_handler);
		int	waitForEvent(ConnectionManager* conn_manager, IActiveEventManager *event_manager);
		void	callEventHandler(ConnectionManager* conn_manager,
										IActiveEventManager* event_manager,
										NetworkIOHandler* io_handler,
										RequestHandler* request_handler);
		void	addActiveEvents(const std::vector<struct pollfd> &pollfds,
								ConnectionManager* conn_manager,
								IActiveEventManager* event_manager);

		std::vector<struct pollfd>	convertToPollfds(const std::map<int, ConnectionData> &connections);
};

#endif