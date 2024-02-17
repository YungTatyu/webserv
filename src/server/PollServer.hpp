#ifndef POLL_SERVER_HPP
#define POLL_SERVER_HPP

#include <vector>
#include <map>

#include "IServer.hpp"
#include "IActiveEventManager.hpp"

class PollServer : public IServer
{
	public:
		PollServer();
		~PollServer();
		void	eventLoop();
		int	waitForEvent();
		void	callEventHander();
		void	addActiveEvents(const std::vector<struct pollfd> &pollfds, IActiveEventManager* event_manager);
		std::vector<struct pollfd>	convertToPollfds(const std::map<int, ConnectionData> &connections);
};

#endif