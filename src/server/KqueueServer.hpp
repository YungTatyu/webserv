#ifndef KQUEUE_SERVER_HPP
#define KQUEUE_SERVER_HPP

#include "conf.hpp"
#include "IServer.hpp"

#if defined(KQUEUE_AVAILABLE)

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <vector>

#include "KqueueActiveEventManager.hpp"

class KqueueServer : public IServer
{
	public:
		KqueueServer();
		~KqueueServer();
		void	eventLoop(ConnectionManager* conn_manager,
							IActiveEventManager* event_manager,
							NetworkIOHandler* io_handler,
							RequestHandler* request_handler,
							ConfigHandler* config_handler);
		int	waitForEvent(ConnectionManager* conn_manager, IActiveEventManager *event_manager);
		void	callEventHandler(ConnectionManager* conn_manager,
									IActiveEventManager* event_manager,
									NetworkIOHandler* io_handler,
									RequestHandler* request_handler,
									ConfigHandler* config_handler);
	private:
		int	kq_; // kqueue fd
		bool	initKqueueServer();
		bool	initKevents(const std::map<int, ConnectionData> &connections);
		int	updateEvent(struct kevent &event, const int event_filter);
		int	deleteEvent(struct kevent &event);
		int	addNewEvent(const int fd, const int event_filter);
};

#endif
#endif
