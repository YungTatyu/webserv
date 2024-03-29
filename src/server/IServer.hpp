#ifndef ISERVER_HPP
#define ISERVER_HPP

#include "ConnectionManager.hpp"
#include "IActiveEventManager.hpp"
#include "NetworkIOHandler.hpp"
#include "RequestHandler.hpp"

class IServer
{
	public:
		virtual ~IServer() {};
		virtual void	eventLoop(ConnectionManager* conn_manager,
								IActiveEventManager* event_manager,
								NetworkIOHandler* io_handler,
								RequestHandler* request_handler) = 0;
		virtual int	waitForEvent(ConnectionManager* conn_manager, IActiveEventManager *event_manager) = 0;
		virtual void	callEventHandler(ConnectionManager* conn_manager,
										IActiveEventManager* event_manager,
										NetworkIOHandler* io_handler,
										RequestHandler* request_handler) = 0;
};

#endif