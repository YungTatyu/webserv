#ifndef POLL_SERVER_HPP
#define POLL_SERVER_HPP

#include <poll.h>
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
							RequestHandler* request_handler,
							ConfigHandler* config_handler,
							TimerTree* timer_tree);
		int	waitForEvent(ConnectionManager* conn_manager, IActiveEventManager *event_manager, TimerTree *time_tree);
		void	callEventHandler(ConnectionManager* conn_manager,
									IActiveEventManager* event_manager,
									NetworkIOHandler* io_handler,
									RequestHandler* request_handler,
									ConfigHandler* config_handler,
									TimerTree* timer_tree);
	private:
		void	addActiveEvents(const std::vector<struct pollfd> &pollfds,
								ConnectionManager* conn_manager,
								IActiveEventManager* event_manager);
		std::vector<struct pollfd>	convertToPollfds(const std::map<int, ConnectionData> &connections);
};

#endif
