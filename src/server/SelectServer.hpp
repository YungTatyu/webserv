#ifndef SELECT_SERVER_HPP
#define SELECT_SERVER_HPP

#include "ConnectionManager.hpp"
#include "IActiveEventManager.hpp"
#include "NetworkIOHandler.hpp"
#include "RequestHandler.hpp"

#include <sys/select.h>

class SelectServer
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
};

#endif