#include "ConnectionManager.hpp"
#include "NetworkIOHandler.hpp"
#include "RequestHandler.hpp"
#include <cstring>

#if defined(EPOLL_AVAILABLE)
#include "EpollServer.hpp"
#include "EpollActiveEventManager.hpp"
#include <sys/epoll.h>

EpollServer::EpollServer() {}

EpollServer::~EpollServer()
{
	close(this->epfd_);
}

void	EpollServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	if (!initEpollServer())
		return;

	if (!initEpollEvent(conn_manager->getConnections()))
		return;

	for ( ; ; )
	{
		waitForEvent(conn_manager, event_manager);

		// 発生したイベントをhandle
		callEventHandler(conn_manager, event_manager, io_handler, request_handler);

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
	}
}

bool	EpollServer::initEpollServer()
{
	// epoll instance 初期化
	// epoll1_create(EPOLL_CLOEXEC);を使用することで、forkでこのfdのオープンを防げる。
	this->epfd_ = epoll_create1(EPOLL_CLOEXEC);
	if (this->epfd_ == -1)
	{
		std::cerr << "webserv: [emerg] epoll_create1 (" << errno << ":" << strerror(errno) << ")\n";
		return false;
	}
	return true;
}

bool	EpollServer::initEpollEvent( const std::map<int, ConnectionData> &connections )
{
	// epoll_event listの作成
	std::vector<struct epoll_event>	event_list(connections.size()); //監視したいevent

	size_t	i = 0;
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin();
		it != connections.end();
		++it)
	{
		event_list[i].events = it->second.event == ConnectionData::READ ? EPOLLIN : EPOLLOUT;
		event_list[i].data.fd = it->first;
		++i;
	}

	for (i = 0 ; i < event_list.size(); ++i)
	{
		if (epoll_ctl(this->epfd_, EPOLL_CTL_ADD, event_list[i].data.fd, &event_list[i]) == -1)
		{
			std::cerr << "webserv: [emerg] epoll_ctl (" << errno << ":"<< strerror(errno) << ")\n";
			return false;
		}
	}

	return true;
}

int	EpollServer::waitForEvent( ConnectionManager* conn_manager, IActiveEventManager* event_manager )
{
	// ここでタイムアウトを設定する必要があるならば、
	// epoll_pwait2()を使うことで、timespec型のtimeout値を指定できる。
	// おそらく必要ないので、epoll_wait()でtimeoutは-1を指定
	// epoll_waitが返すエラー
	// EINTR -> signal
	// EFAULT -> eventsによって指し示されたメモリが書き込み権限無し、またはメモリ不足。この場合再実行？
	std::vector<struct epoll_event>	*active_events = static_cast<std::vector<struct epoll_event>*>(event_manager->getActiveEvents());

	active_events->resize(conn_manager->getConnections().size());

	int size = epoll_wait(this->epfd_, active_events->data(), active_events->size(), -1);
	event_manager->setActiveEventsNum(size);
	return size;
}

void	EpollServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	// event handling
	std::vector<struct epoll_event>	*active_events_ptr =
		static_cast<std::vector<struct epoll_event>*>(event_manager->getActiveEvents());
	std::vector<struct epoll_event>	&active_events = *active_events_ptr;

	// 発生したイベントの数だけloopする
	for (int i = 0; i < event_manager->getActiveEventsNum(); ++i)
	{
		int	status = RequestHandler::NONE;
		if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleReadEvent(*io_handler, *conn_manager, active_events[i].data.fd);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleWriteEvent(*io_handler, *conn_manager, active_events[i].data.fd);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleErrorEvent(*io_handler, *conn_manager, active_events[i].data.fd);
		
		// kqueueで監視しているイベント情報を更新
		switch (status)
		{
		case RequestHandler::UPDATE_READ:
			updateEvent(active_events[i], EPOLLIN);
			break;

		case RequestHandler::UPDATE_WRITE:
			updateEvent(active_events[i], EPOLLOUT);
			break;

		case RequestHandler::UPDATE_CLOSE:
			deleteEvent(active_events[i]);
			break;

		default:
			if (status >= 0) // fdだったら
				addNewEvent(status, EPOLLIN);
			break;
		}
	}

}

int	EpollServer::addNewEvent( const int fd, const int event_filter )
{
	struct epoll_event	new_event;
	new_event.events = event_filter;
	new_event.data.fd = fd;
	return epoll_ctl(this->epfd_, EPOLL_CTL_ADD, new_event.data.fd, &new_event);
}

int	EpollServer::updateEvent( struct epoll_event &old_event, const int event_filter )
{
	old_event.events = event_filter;
	return epoll_ctl(this->epfd_, EPOLL_CTL_MOD, old_event.data.fd, &old_event);
}

int	EpollServer::deleteEvent( struct epoll_event &old_event )
{
	return epoll_ctl(this->epfd_, EPOLL_CTL_DEL, old_event.data.fd, NULL);
}

#endif
