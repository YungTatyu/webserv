#ifndef SELECT_SERVER_HPP
#define SELECT_SERVER_HPP

#include "ConnectionManager.hpp"
#include "NetworkIOHandler.hpp"
#include "RequestHandler.hpp"
#include "SelectActiveEventManager.hpp"
#include "IServer.hpp"

#include <sys/select.h>

class SelectServer : public IServer
{
	public:
		SelectServer();
		~SelectServer();
		void	eventLoop(ConnectionManager* conn_manager,
								IActiveEventManager* event_manager,
								NetworkIOHandler* io_handler,
								RequestHandler* request_handler);
		int	waitForEvent(ConnectionManager* conn_manager, IActiveEventManager *event_manager);
		void	callEventHandler(ConnectionManager* conn_manager,
										IActiveEventManager* event_manager,
										NetworkIOHandler* io_handler,
										RequestHandler* request_handler);
		int	addSocketToSets(const std::map<int, ConnectionData> &connections);
		void	addActiveEvents(const std::map<int, ConnectionData> &connections, IActiveEventManager *event_manager);
		fd_set	read_set_;
		fd_set	write_set_;
};

#endif