#include "SelectServer.hpp"
#include <algorithm>

SelectServer::SelectServer() {}

SelectServer::~SelectServer() {}

void	SelectServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler,
	ConfigHandler* config_handler,
	TimerTree* timer_tree
)
{
	for ( ; ; )
	{
		waitForEvent(conn_manager, event_manager, timer_tree);

		callEventHandler(conn_manager, event_manager, io_handler, request_handler, config_handler, timer_tree);

		event_manager->clearAllEvents();
	}
}

int	SelectServer::waitForEvent(ConnectionManager*conn_manager, IActiveEventManager *event_manager, TimerTree *timer_tree)
{
	const int max_fd = addSocketToSets(*conn_manager);
	// 現在時刻を更新
	Timer::updateCurrentTime();
	// TODO: select serverではマクロFD_SETSIZE以上のfdを監視できない
	// TODO: error処理どうするべきか、retryする？
	struct timeval	tv = timer_tree->findTimeval();
	struct timeval	*tvp = &tv;
	if (tv.tv_sec == -1 && tv.tv_usec == -1)
		tvp = NULL;
	int re = select(max_fd + 1, &(this->read_set_), &(this->write_set_), NULL, tvp);
	addActiveEvents(conn_manager->getConnections(), event_manager);

	return re;
}

int	SelectServer::addSocketToSets(const ConnectionManager& conn_manager)
{
	const std::map<int, ConnectionData*>&	connections = conn_manager.getConnections();
	int	max_fd = 0;
	FD_ZERO(&(this->read_set_));
	FD_ZERO(&(this->write_set_));

	for (std::map<int, ConnectionData*>::const_iterator it = connections.begin();
		it != connections.end();
		++it
	)
	{
		const ConnectionData	&connection = *(it->second);
		const int	fd = it->first;
		switch (connection.event)
		{
		case ConnectionData::EV_CGI_READ:
		case ConnectionData::EV_CGI_WRITE:
			// cgi eventの時は、クライアントsocketはイベント登録しない
			if (!conn_manager.isCgiSocket(fd))
				continue;
			break;
		default:
			break;
		}
		switch (connection.event)
		{
		case ConnectionData::EV_READ:
		case ConnectionData::EV_CGI_READ:
			FD_SET(fd, &(this->read_set_));
			break;
		case ConnectionData::EV_WRITE:
		case ConnectionData::EV_CGI_WRITE:
			FD_SET(fd, &(this->write_set_));
			break;
		}
		max_fd = std::max(max_fd, fd);
	}
	return max_fd;
}

void	SelectServer::addActiveEvents(
	const std::map<int, ConnectionData*> &connections,
	IActiveEventManager *event_manager
)
{
	for (std::map<int, ConnectionData*>::const_iterator it = connections.begin();
		it != connections.end();
		++it
	)
	{
		int	fd = it->first;
		if (FD_ISSET(fd, &(this->read_set_)))
		{
			const SelectEvent	event(fd, SelectEvent::SELECT_READ);
			event_manager->addEvent(static_cast<const void*>(&event));
		}
		else if (FD_ISSET(fd, &(this->write_set_)))
		{
			const SelectEvent	event(fd, SelectEvent::SELECT_WRITE);
			event_manager->addEvent(static_cast<const void*>(&event));
		}
	}
}

void	SelectServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler,
	ConfigHandler* config_handler,
	TimerTree* timer_tree
)
{
	const std::vector<SelectEvent> *active_events_ptr =
		static_cast<std::vector<SelectEvent>*>(event_manager->getActiveEvents());
	const std::vector<SelectEvent> active_events = *active_events_ptr;

		// TimeoutEvent発生
	if (event_manager->getActiveEventsNum() == 0)
	{
		request_handler->handleTimeoutEvent(*io_handler, *conn_manager, *config_handler, *timer_tree);
		return;
	}

	// 現在時刻を更新
	Timer::updateCurrentTime();

	for (size_t i = 0; i < active_events.size(); ++i)
	{
		if (event_manager->isReadEvent(static_cast<const void*>(&active_events[i])))
			request_handler->handleReadEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].fd_);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&active_events[i])))
			request_handler->handleWriteEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].fd_);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&active_events[i])))
			request_handler->handleWriteEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].fd_);

	}

}
