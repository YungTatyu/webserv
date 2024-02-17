#ifndef ISERVER_HPP
#define ISERVER_HPP

#include "ConnectionManager.hpp"
#include "IActiveEventManager.hpp"

class IServer
{
	public:
		virtual ~IServer() {};
		virtual void	eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager) = 0;
		virtual int	waitForEvent() = 0;
		virtual void	callEventHander() = 0;
};

#endif