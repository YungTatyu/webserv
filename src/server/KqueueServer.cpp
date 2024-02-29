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
		std::cout << "waiting events\n";
		waitForEvent(conn_manager, event_manager);
		std::cout << "events occured\n";

		// 発生したイベントをhandleする
		callEventHandler(conn_manager, event_manager, io_handler, request_handler);
		std::cout << "events handled\n";

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
		std::cout << "active events deleted\n";

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
	// static bool	init = true;

	static std::vector<struct kevent>	event_list; // 監視したいevent
	std::vector<struct kevent>	*active_events =
		static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());

	// event loop1回目はすべてのイベントを監視リストに追加
	// if (init)
	initKevents(event_list, conn_manager->getConnections());
	// else
	// 	AddNewEvents(event_list, conn_manager->getConnections());
	// init = false;

	// 発生したeventをすべて格納できるサイズにする
	active_events->resize(event_list.size());
	// TODO: error処理どうするか？ server downさせる？
	int re = kevent(this->kq_, event_list.data(), event_list.size(), active_events->data(), active_events->size(), NULL);
	std::cout << "event counts=" << re << "\n";
	event_manager->setActiveEventsNum(re);
	return re;
}

void	KqueueServer::initKevents(
	std::vector<struct kevent>	&event_list,
	const std::map<int, ConnectionData> &connections
)
{
	// 監視するeventの数だけ、要素を確保する
	event_list.resize(connections.size());

	size_t	i = 0;
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin();
		it != connections.end();
		++it)
	{
		const int	event_filter = it->second.event == ConnectionData::READ ? EVFILT_READ : EVFILT_WRITE;
		// TODO: flag詳細調べる
		EV_SET(&event_list[i], it->first, event_filter, EV_ADD|EV_ENABLE|EV_CLEAR, 0, 0, 0);
		// std::cout << "event addr=" << &event_list[i] << "\n";
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
	std::vector<struct kevent>	*active_events_ptr =
		static_cast<std::vector<struct kevent>*>(event_manager->getActiveEvents());
	std::vector<struct kevent>	&active_events = *active_events_ptr;

	// 発生したイベントの数だけloopする
	for (int i = 0; i < event_manager->getActiveEventsNum(); ++i)
	{
		// RequestHandler::UPDATE_STATUS	status = RequestHandler::NONE;
		if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
			request_handler->handleReadEvent(*io_handler, *conn_manager, active_events[i].ident);
			// status = request_handler->handleReadEvent(*io_handler, *conn_manager, active_events[i].ident);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
			request_handler->handleWriteEvent(*io_handler, *conn_manager, active_events[i].ident);
			// status = request_handler->handleWriteEvent(*io_handler, *conn_manager, active_events[i].ident);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
			request_handler->handleErrorEvent(*io_handler, *conn_manager, active_events[i].ident);
			// status = request_handler->handleErrorEvent(*io_handler, *conn_manager, active_events[i].ident);
		
		// kqueueで監視しているイベント情報を更新
		// switch (status)
		// {
		// case RequestHandler::UPDATE_READ:
		// 	updateEvent(active_events[i], EVFILT_READ);
		// 	break;

		// case RequestHandler::UPDATE_WRITE:
		// 	std::cout << "update: write=" << &active_events[i] << "\n";
		// 	updateEvent(active_events[i], EVFILT_WRITE);
		// 	break;

		// case RequestHandler::UPDATE_CLOSE:
		// 	deleteEvent(active_events[i]);
		// 	break;

		// default:
		// 	// std::cout << "default\n";
		// 	break;
		// }
	}
}

/**
 * @brief kqueueで監視するイベントをupdate
 * 
 * @param event 
 * @param event_filter 
 */
void	KqueueServer::updateEvent(struct kevent &event, const int event_filter)
{
	EV_SET(&event, event.ident, event_filter, EV_ADD, 0, 0, 0);
}

/**
 * @brief kqueueで監視するイベントから削除
 * 
 * @param event 
 */
void	KqueueServer::deleteEvent(struct kevent &event)
{
	EV_SET(&event, event.ident, event.filter, EV_DELETE, 0, 0, 0);
}

void	KqueueServer::AddNewEvents(
	std::vector<struct kevent> &event_list,
	const std::map<int, ConnectionData> &connections)
{
	// TODO: 新しいイベントのみを追加するロジックを追加する
	// refactor時に対応する
	initKevents(event_list, connections);
}

#endif