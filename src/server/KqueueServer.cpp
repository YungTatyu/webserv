#include "KqueueServer.hpp"
#if defined(KQUEUE_AVAILABLE)

#include <stdio.h>

KqueueServer::KqueueServer() {}

KqueueServer::~KqueueServer() {}

void	KqueueServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	if (!initKqueueServer())
		return;

	for ( ; ; )
	{
		waitForEvent(conn_manager, event_manager);

		// 発生したイベントをhandleする
		callEventHandler(conn_manager, event_manager, io_handler, request_handler);

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

int	KqueueServer::waitForEvent(ConnectionManager*conn_manager, IActiveEventManager *event_manager)
{
	std::vector<struct kevent>	event_list; // 監視したいevent
	std::vector<struct kevent>	*active_events =
		static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());

	initKevents(event_list, conn_manager->getConnections());

	// 発生したeventをすべて格納できるサイズにする
	active_events->reserve(event_list.size());
	// TODO: error処理どうするか？ server downさせる？
	int re = kevent(this->kq_, event_list.data(), event_list.size(), active_events->data(), active_events->size(), NULL);
	event_manager->setActiveEventsNum(re);
	return re;
}

void	KqueueServer::initKevents(
	std::vector<struct kevent>	&event_list,
	const std::map<int, ConnectionData> &connections
)
{
	// 監視するeventの数だけ、要素を確保する
	event_list.reserve(connections.size());

	size_t	i = 0;
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin();
		it != connections.end();
		++it)
	{
		const int	event_flag = it->second.event == ConnectionData::READ ? EVFILT_READ : EVFILT_WRITE;
		// TODO: flag詳細調べる
		EV_SET(&event_list[i], it->first, event_flag, EV_ADD|EV_ENABLE, 0, 0, 0);
		++i;
	}
}

void	KqueueServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	const std::vector<struct kevent>	*active_events_ptr =
		static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());
	const std::vector<struct kevent>	&active_events = *active_events_ptr;

	// 発生したイベントの数だけloopする
	for (int i = 0; i < event_manager->getActiveEventsNum(); ++i)
	{
		if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
			request_handler->handleReadEvent(*io_handler, *conn_manager, active_events[i].ident);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
			request_handler->handleWriteEvent(*io_handler, *conn_manager, active_events[i].ident);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
			request_handler->handleErrorEvent(*io_handler, *conn_manager, (active_events[i].ident));
	}
}

#endif