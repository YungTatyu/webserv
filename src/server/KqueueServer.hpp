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
		bool	initKqueueServer();
		void	initKevents(std::vector<struct kevent> &event_list, const std::map<int, ConnectionData> &connections);
		void	eventLoop(ConnectionManager* conn_manager,
							IActiveEventManager* event_manager,
							NetworkIOHandler* io_handler,
							RequestHandler* request_handler);
		int	waitForEvent(ConnectionManager* conn_manager, IActiveEventManager *event_manager);
		void	callEventHandler(ConnectionManager* conn_manager,
									IActiveEventManager* event_manager,
									NetworkIOHandler* io_handler,
									RequestHandler* request_handler);
		void	updateEvent(struct kevent &event, const int event_filter);
		void	deleteEvent(struct kevent &event);
		void	AddNewEvents(std::vector<struct kevent> &event_list, const std::map<int, ConnectionData> &connections);
	private:
		int	kq_; // kqueue fd
};

#endif
#endif