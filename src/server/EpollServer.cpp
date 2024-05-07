#include <cstring>

#include "ConnectionManager.hpp"
#include "NetworkIOHandler.hpp"
#include "RequestHandler.hpp"

#if defined(EPOLL_AVAILABLE)
#include <sys/epoll.h>

#include "EpollActiveEventManager.hpp"
#include "EpollServer.hpp"

EpollServer::EpollServer() {}

EpollServer::~EpollServer() { close(this->epfd_); }

void EpollServer::eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                            NetworkIOHandler* io_handler, RequestHandler* request_handler,
                            ConfigHandler* config_handler, TimerTree* timer_tree) {
  if (!initEpollServer()) return;

  if (!initEpollEvent(conn_manager->getConnections())) return;

  for (;;) {
    waitForEvent(conn_manager, event_manager, timer_tree);

    // 発生したイベントをhandle
    callEventHandler(conn_manager, event_manager, io_handler, request_handler, config_handler, timer_tree);

    // 発生したすべてのイベントを削除
    event_manager->clearAllEvents();
  }
}

bool EpollServer::initEpollServer() {
  // epoll instance 初期化
  // epoll1_create(EPOLL_CLOEXEC);を使用することで、forkでこのfdのオープンを防げる。
  this->epfd_ = epoll_create1(EPOLL_CLOEXEC);
  if (this->epfd_ == -1) {
    std::cerr << "webserv: [emerg] epoll_create1 (" << errno << ":" << strerror(errno) << ")\n";
    return false;
  }
  return true;
}

bool EpollServer::initEpollEvent(const std::map<int, ConnectionData*>& connections) {
  // 監視したいイベントを追加
  for (std::map<int, ConnectionData*>::const_iterator it = connections.begin(); it != connections.end();
       ++it) {
    struct epoll_event ep;
    ep.events = it->second->event == ConnectionData::EV_READ ? EPOLLIN : EPOLLOUT;
    ep.data.fd = it->first;

    if (epoll_ctl(this->epfd_, EPOLL_CTL_ADD, ep.data.fd, &ep) == -1) {
      std::cerr << "webserv: [emerg] epoll_ctl (" << errno << ":" << strerror(errno) << ")\n";
      return false;
    }
  }

  return true;
}

void	EpollServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler,
	ConfigHandler* config_handler,
	TimerTree* timer_tree
)
{
	// event handling
	std::vector<struct epoll_event>	*active_events_ptr =
		static_cast<std::vector<struct epoll_event>*>(event_manager->getActiveEvents());
	std::vector<struct epoll_event>	&active_events = *active_events_ptr;

	// TimeoutEvent発生
	if (event_manager->getActiveEventsNum() == 0)
	{
		request_handler->handleTimeoutEvent(*io_handler, *conn_manager, *config_handler, *timer_tree);
		return;
	}

	// 現在時刻を更新
	Timer::updateCurrentTime();

	// 発生したイベントの数だけloopする
	for (int i = 0; i < event_manager->getActiveEventsNum(); ++i)
	{

		const cgi::CGIHandler&	cgi_handler = conn_manager->getCgiHandler(active_events[i].data.fd);
		const bool	is_cgi_sock = conn_manager->isCgiSocket(active_events[i].data.fd);
		const int	cli_sock = cgi_handler.getCliSocket(); // cgi socketの場合に使用する

		int	status = RequestHandler::UPDATE_NONE;
		if (event_manager->isReadEvent(static_cast<const void*>(&(active_events[i])), is_cgi_sock))
			status = request_handler->handleReadEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].data.fd);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(active_events[i])), is_cgi_sock))
			status = request_handler->handleWriteEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].data.fd);
		else if (event_manager->isEofEvent(static_cast<const void*>(&(active_events[i])), is_cgi_sock))
			status = request_handler->handleEofEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, active_events[i].data.fd);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(active_events[i]))))
			status = request_handler->handleErrorEvent(*io_handler, *conn_manager, *timer_tree, active_events[i].data.fd);
		
		// epoll_fdで監視しているイベント情報を更新
		switch (status)
		{
		case RequestHandler::UPDATE_READ:
			updateEvent(active_events[i], EPOLLIN);
			break;

		case RequestHandler::UPDATE_WRITE:
			if (is_cgi_sock)
			{
				deleteEvent(active_events[i]); // cgi socketを監視から削除する: cgi socketをcloseした後だから呼ばなくてもいい
				addNewEvent(cli_sock, EPOLLOUT);
				break;
			}
			updateEvent(active_events[i], EPOLLOUT);
			break;

		case RequestHandler::UPDATE_CLOSE:
			deleteEvent(active_events[i]);
			break;

		case RequestHandler::UPDATE_CGI_READ:
			if (is_cgi_sock)
			{
				updateEvent(active_events[i], EPOLLIN);
				break;
			}
			deleteEvent(active_events[i]); // client socketを監視から一時的に削除する
			addNewEvent(cgi_handler.getCgiSocket(), EPOLLIN);
			break;

		case RequestHandler::UPDATE_CGI_WRITE:
			deleteEvent(active_events[i]); // client socketを監視から一時的に削除する
			addNewEvent(cgi_handler.getCgiSocket(), EPOLLOUT);
			break;

		default:
			if (status >= 0) // fdだったら
				addNewEvent(status, EPOLLIN);
			break;
		}
	}
}

int EpollServer::waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                              TimerTree* timer_tree) {
  // ここでタイムアウトを設定する必要があるならば、
  // epoll_pwait2()を使うことで、timespec型のtimeout値を指定できる。
  // おそらく必要ないので、epoll_wait()でtimeoutは-1を指定
  // epoll_waitが返すエラー
  // EINTR -> signal
  // EFAULT -> eventsによって指し示されたメモリが書き込み権限無し、またはメモリ不足。この場合再実行？
  std::vector<struct epoll_event>* active_events =
      static_cast<std::vector<struct epoll_event>*>(event_manager->getActiveEvents());

  active_events->resize(conn_manager->getConnections().size());

  // 現在時刻を更新
  Timer::updateCurrentTime();
  int size = epoll_wait(this->epfd_, active_events->data(), active_events->size(), timer_tree->findTimer());
  event_manager->setActiveEventsNum(size);
  return size;
}

int EpollServer::addNewEvent(const int fd, const uint32_t event_filter) {
  struct epoll_event new_event;
  new_event.events = event_filter;
  new_event.data.fd = fd;
  return epoll_ctl(this->epfd_, EPOLL_CTL_ADD, new_event.data.fd, &new_event);
}

int EpollServer::updateEvent(struct epoll_event& old_event, const uint32_t event_filter) {
  old_event.events = event_filter;
  return epoll_ctl(this->epfd_, EPOLL_CTL_MOD, old_event.data.fd, &old_event);
}

int EpollServer::deleteEvent(struct epoll_event& old_event) {
  return epoll_ctl(this->epfd_, EPOLL_CTL_DEL, old_event.data.fd, NULL);
}

#endif
