#include "SelectServer.hpp"
#include <algorithm>

SelectServer::SelectServer() {}

SelectServer::~SelectServer() {}

void	SelectServer::initSelectServer()
{
	FD_ZERO(&(this->read_set_));
	FD_ZERO(&(this->write_set_));
}

void	SelectServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	for ( ; ; )
	{
		std::cout << "waiting event" << std::endl;
		waitForEvent(conn_manager, event_manager);

		std::cout << "calling eventhandler" << std::endl;
		callEventHandler(conn_manager, event_manager, io_handler, request_handler);

		event_manager->clearAllEvents();
		std::cout << "clear event" << std::endl;
	}
}

int	SelectServer::waitForEvent(ConnectionManager*conn_manager, IActiveEventManager *event_manager)
{
	const int max_fd = addSocketToSets(conn_manager->getConnections());
	// TODO: error処理どうするべきか、retryする？
	int re = select(max_fd + 1, &(this->read_set_), &(this->write_set_), NULL, NULL);
	std::cout << "event counts:" << re << std::endl;
	addActiveEvents(conn_manager->getConnections(), event_manager);

	return re;
}

int	SelectServer::addSocketToSets(const std::map<int, ConnectionData> &connections)
{
	int	max_fd = 0;
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin();
		it != connections.end();
		++it
	)
	{
		const int	fd = it->first;
		std::cout << "fd=" << fd << std::endl;
		const ConnectionData	&connection = it->second;
		switch (connection.event)
		{
		case ConnectionData::READ:
			FD_ZERO(&(this->read_set_));
			FD_SET(fd, &(this->read_set_));
			break;
		
		case ConnectionData::WRITE:
			FD_ZERO(&(this->write_set_));
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
			std::cout << "read event" << std::endl;
			const SelectEvent	event(fd, SelectEvent::SELECT_READ);
			event_manager->addEvent(static_cast<const void*>(&event));
		}
		else if (FD_ISSET(fd, &(this->write_set_)))
		{
			std::cout << "write event" << std::endl;
			const SelectEvent	event(fd, SelectEvent::SELECT_WRITE);
			event_manager->addEvent(static_cast<const void*>(&event));
		}
	}
}

void	SelectServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	const std::vector<SelectEvent> *active_events_ptr =
		static_cast<std::vector<SelectEvent>*>(event_manager->getActiveEvents());
	const std::vector<SelectEvent> active_events = *active_events_ptr;

	for (size_t i = 0; i < active_events.size(); ++i)
	{
		const SelectEvent	event = active_events[i];

		switch (event.event_)
		{
		case SelectEvent::SELECT_READ:
			request_handler->handleReadEvent(*io_handler, *conn_manager, event.fd_);
			break;
		
		case SelectEvent::SELECT_WRITE:
			request_handler->handleWriteEvent(*io_handler, *conn_manager, event.fd_);
			break;
		
		default:
			break;
		}
	}
}