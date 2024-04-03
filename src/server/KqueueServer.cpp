#include "KqueueServer.hpp"
#if defined(KQUEUE_AVAILABLE)

#include <stdio.h>

KqueueServer::KqueueServer() {}

KqueueServer::~KqueueServer()
{
	close(this->kq_);
}

void	KqueueServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler,
	ConfigHandler* config_handler
)
{
	if (!initKqueueServer())
		return;
	if (!initKevents(conn_manager->getConnections()))
		return;
	for ( ; ; )
	{
		waitForEvent(conn_manager, event_manager);

		// 発生したイベントをhandleする
		callEventHandler(conn_manager, event_manager, io_handler, request_handler, config_handler);

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
	}
}

bool	KqueueServer::initKqueueServer()
{
	this->kq_ = kqueue();
	if (this->kq_ == -1)
	{
		std::cerr << "webserv: [emerg] kqueue (" << errno << ":"<< strerror(errno) << ")\n";
		return false;
	}
	return true;
}

bool	KqueueServer::initKevents(const std::map<int, ConnectionData*> &connections)
{
	std::vector<struct kevent>	event_list;// 監視したいevent

	// 監視するeventの数だけ、要素を確保する
	event_list.resize(connections.size());

	size_t	i = 0;
	for (std::map<int, ConnectionData*>::const_iterator it = connections.begin();
		it != connections.end();
		++it)
	{
		const int	event_filter = it->second->event == ConnectionData::EV_READ ? EVFILT_READ : EVFILT_WRITE;
		EV_SET(&event_list[i], it->first, event_filter, EV_ADD|EV_ENABLE, 0, 0, 0);
		++i;
	}
	int re = kevent(this->kq_, event_list.data(), event_list.size(), NULL, 0, NULL);
	if (re == -1)
	{
		std::cerr << "webserv: [emerg] kevent (" << errno << ":"<< strerror(errno) << ")\n";
		return false;
	}
	return true;
}

int	KqueueServer::waitForEvent(ConnectionManager*conn_manager, IActiveEventManager *event_manager)
{
	std::vector<struct kevent>	*active_events =
		static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());

	// 発生したeventをすべて格納できるサイズにする
	active_events->resize(conn_manager->getConnections().size());
	// TODO: error処理どうするか？ server downさせる？
	int re = kevent(this->kq_, NULL, 0, active_events->data(), active_events->size(), NULL);
	event_manager->setActiveEventsNum(re);
	return re;
}

void	KqueueServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler,
	ConfigHandler* config_handler
)
{
	std::vector<struct kevent>	*active_events_ptr =
		static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());
	std::vector<struct kevent>	&active_events = *active_events_ptr;

	// 発生したイベントの数だけloopする
	for (int i = 0; i < event_manager->getActiveEventsNum(); ++i)
	{
		int	status = RequestHandler::UPDATE_NONE;
		if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleReadEvent(*io_handler, *conn_manager, *config_handler, active_events[i].ident);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleWriteEvent(*io_handler, *conn_manager, active_events[i].ident);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleErrorEvent(*io_handler, *conn_manager, active_events[i].ident);
		
		// kqueueで監視しているイベント情報を更新
		switch (status)
		{
		case RequestHandler::UPDATE_READ:
			updateEvent(active_events[i], EVFILT_READ);
			break;
		case RequestHandler::UPDATE_WRITE:
		{
			if (conn_manager->isCgiSocket(active_events[i].ident))
			{
				const cgi::CGIHandler&	cgi_handler = conn_manager->getCgiHandler(active_events[i].ident);
				deleteEvent(active_events[i]); // cgi socketを監視から削除する
				addNewEvent(cgi_handler.getCliSocket(), EVFILT_WRITE);
			}
			else
				updateEvent(active_events[i], EVFILT_WRITE);
			break;
		}
		case RequestHandler::UPDATE_CLOSE:
			// deleteEvent(active_events[i]);
			break;
		case RequestHandler::UPDATE_CGI_READ:
			if (conn_manager->isCgiSocket(active_events[i].ident))
				updateEvent(active_events[i], EVFILT_READ);
			else
			{
				const cgi::CGIHandler&	cgi_handler = conn_manager->getCgiHandler(active_events[i].ident);
				deleteEvent(active_events[i]); // client socketを監視から一時的に削除する
				addNewEvent(cgi_handler.getCgiSocket(), EVFILT_READ);
			}
			break;
		case RequestHandler::UPDATE_CGI_WRITE:
		{
			const cgi::CGIHandler&	cgi_handler = conn_manager->getCgiHandler(active_events[i].ident);
			deleteEvent(active_events[i]); // client socketを監視から一時的に削除する
			addNewEvent(cgi_handler.getCgiSocket(), EVFILT_WRITE);
			break;
		}
		default:
			if (status >= 0) // fdだったら
				addNewEvent(status, EVFILT_READ);
			break;
		}
	}
}

/**
 * @brief kqueueで監視するイベントをupdate
 * 
 * @param event 
 * @param event_filter 
 */
int	KqueueServer::updateEvent(struct kevent &old_event, const int event_filter)
{
	const int	fd = old_event.ident;
	deleteEvent(old_event);
	return addNewEvent(fd, event_filter);
}

/**
 * @brief kqueueで監視するイベントから削除
 * 
 * @param event 
 */
int	KqueueServer::deleteEvent(struct kevent &event)
{
	EV_SET(&event, event.ident, event.filter, EV_DELETE, event.fflags, event.data, event.udata);
	int re = kevent(this->kq_, &event, 1, NULL, 0, NULL);
	return re;
}

int	KqueueServer::addNewEvent(const int fd, const int event_filter)
{
	struct kevent	event;
	EV_SET(&event, fd, event_filter, EV_ADD|EV_ENABLE, 0, 0, 0);
	int re = kevent(this->kq_, &event, 1, NULL, 0, NULL);
	return re;
}

#endif
