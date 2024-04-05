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
	const int max_fd = addSocketToSets(conn_manager->getConnections());
	// 現在時刻を更新
	Timer::updateCurrentTime();
	// TODO: select serverではマクロFD_SETSIZE以上のfdを監視できない
	// TODO: error処理どうするべきか、retryする？
	struct timeval	tv = timer_tree->findTimeval();
	int re = select(max_fd + 1, &(this->read_set_), &(this->write_set_), NULL, &tv);
	addActiveEvents(conn_manager->getConnections(), event_manager);

	return re;
}

int	SelectServer::addSocketToSets(const std::map<int, ConnectionData> &connections)
{
	int	max_fd = 0;
	FD_ZERO(&(this->read_set_));
	FD_ZERO(&(this->write_set_));

	for (std::map<int, ConnectionData>::const_iterator it = connections.begin();
		it != connections.end();
		++it
	)
	{
		const int	fd = it->first;
		const ConnectionData	&connection = it->second;
		switch (connection.event)
		{
		case ConnectionData::EV_READ:
			FD_SET(fd, &(this->read_set_));
			break;
		
		case ConnectionData::EV_WRITE:
			FD_SET(fd, &(this->write_set_));
			break;

		default:
			break;
		}
		max_fd = std::max(max_fd, fd);
	}
	return max_fd;
}

void	SelectServer::addActiveEvents(
	const std::map<int, ConnectionData> &connections,
	IActiveEventManager *event_manager
)
{
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin();
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
		int status = RequestHandler::NONE;
		if (event_manager->isReadEvent(static_cast<const void*>(&active_events[i])))
			status = request_handler->handleReadEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].fd_);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&active_events[i])))
			status = request_handler->handleWriteEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].fd_);

		// kqueueで監視しているイベント情報を更新
		config::Time timeout;
		switch (status)
		{
		case RequestHandler::UPDATE_WRITE:
			// keepaliveが無効なので接続を閉じる
			if (!timer_tree->timerExists(active_events[i].fd_))
				io_handler->closeConnection(*conn_manager, active_events[i].fd_);
			break;

		default:
			break;
		}
	}

}
