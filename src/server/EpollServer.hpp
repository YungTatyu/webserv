#ifndef EPOLL_SERVER_HPP
# define EPOLL_SERVER_HPP

#if defined(EPOLL_AVAILABLE)

# include "ConnectionManager.hpp"
# include "NetworkIOHandler.hpp"
# include "RequestHandler.hpp"
# include "SelectActiveEventManager.hpp"
# include "IServer.hpp"


class EpollServer : public IServer
{
	public:
		EpollServer();
		~EpollServer();
		void	eventLoop( ConnectionManager* conn_manager,
							IActiveEventManager* event_manager,
							NetworkIOHandler* io_handler,
							RequestHandler* request_handler );
		int	waitForEvent( ConnectionManager* conn_manager, IActiveEventManager *event_manager );
		void	callEventHandler( ConnectionManager* conn_manager,
								IActiveEventManager* event_manager,
								NetworkIOHandler* io_handler,
								RequestHandler* request_handler );
		int	addSocketToSets( const std::map<int, ConnectionData> &connections );
		void	addActiveEvents( const std::map<int, ConnectionData> &connections, IActiveEventManager *event_manager );
	private:
		int	epfd_; // epoll instance
		bool	initEpollServer();
		bool	initEpollEvent();
		int	updateEvent();
		int	deleteEvent();
		int	addNewEvent();
};

#endif
#endif
