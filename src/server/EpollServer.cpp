#include "EpollServer.hpp"

#if defind(EPOLL_AVAILABLE)

EpollServer::EpollServer() {}

EpollServer::~EpollServer()
{
	close(this->ep_);
}

void	EpollServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
}

bool	EpollServer::initEpollServer()
{
	// epoll instance 初期化
	// epoll1_create(EPOLL_CLOEXEC);を使用することで、forkでこのfdのオープンを防げる。
	return true;
}

bool	EpollServer::initEpollEvent(const std::map<int, ConnectionData> &connections)
{
	// epoll_event listの作成
	return true;
}

int	EpollServer::waitForEvent( ConnectionManager* conn_manager, IactiveEventManager* event_manager)
{
	// ここでタイムアウトを設定する必要があるならば、
	// epoll_pwait2()を使うことで、timespec型のtimeout値を指定できる。
	// おそらく必要ないので、epoll_wait()でtimeoutは-1を指定
	// epoll_waitが返すエラー
	// EINTR -> signal
	// EFAULT -> eventsによって指し示されたメモリが書き込み権限無し、またはメモリ不足。この場合再実行？
	return 0;
}

void	EpollServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	// event handling
}

int	EpollServer::updateEvent( struct epoll_event &old_event, const int event_filter )
{
	// epoll_ctl()
	// EPOLL_CTL_MODを第二引数に指定することで既存のイベントを変更可能
	return 1;
}

int	EpollServer::deleteEvent( struct epoll_event &old_event, const int event_filter )
{
	// epoll_ctl()
	// EPOLL_CTL_DEL
	return 1;
}

int	EpollServer::addNewEvent( struct epoll_event &old_event, const int event_filter )
{
	// epoll_ctl()
	// EPOLL_CTL_ADD
	return 1;
}

#endif
