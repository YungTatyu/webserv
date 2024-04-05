#include "PollServer.hpp"

PollServer::PollServer() {}

PollServer::~PollServer() {}

void	PollServer::eventLoop(
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

		// 発生したイベントをhandleする
		callEventHandler(conn_manager, event_manager, io_handler, request_handler, config_handler, timer_tree);

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
	}
}

int	PollServer::waitForEvent(ConnectionManager*conn_manager, IActiveEventManager *event_manager, TimerTree *timer_tree)
{
	(void)timer_tree;
	std::vector<pollfd> pollfds = convertToPollfds(conn_manager->getConnections());

	// 現在時刻を更新
	Timer::updateCurrentTime();

	// TODO: error起きたときどうしようか? 一定数retry? serverはdownしたらダメな気がする
	int re = poll(pollfds.data(), pollfds.size(), timer_tree->findTimer());

	// 発生したイベントをActiveEventManagerにすべて追加
	addActiveEvents(pollfds, conn_manager, event_manager);
	return re;
}

void	PollServer::addActiveEvents(
	const std::vector<pollfd> &pollfds,
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager
)
{
	const size_t	size = conn_manager->getConnections().size();
	for (size_t i = 0; i < size; ++i)
	{
		const struct pollfd& cur_pfd = pollfds[i];
		// イベントが発生していたら、active_eventに追加
		if (event_manager->isReadEvent(static_cast<const void*>(&cur_pfd))
			|| event_manager->isWriteEvent(static_cast<const void*>(&cur_pfd))
			|| event_manager->isErrorEvent(static_cast<const void*>(&cur_pfd))
		)
			event_manager->addEvent(static_cast<const void*>(&cur_pfd));
	}
}

void	PollServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler,
	ConfigHandler* config_handler,
	TimerTree* timer_tree
)
{
	const std::vector<pollfd> *active_events =
		static_cast<const std::vector<pollfd>*>(event_manager->getActiveEvents());

		// TimeoutEvent発生
	if (event_manager->getActiveEventsNum() == 0)
	{
		request_handler->handleTimeoutEvent(*io_handler, *conn_manager, *config_handler, *timer_tree);
		return;
	}

	// 現在時刻を更新
	Timer::updateCurrentTime();

	// 発生したイベントの数だけloopする
	for (std::vector<pollfd>::const_iterator it = active_events->begin();
		it != active_events->end();
		++it
	)
	{
		// 発生したeventに対するhandlerを呼ぶ
		// interfaceを実装したことにより、関数ポインタのmapが使えなくなった・・・　どうしよう？？？
		int status = RequestHandler::NONE;
		if (event_manager->isReadEvent(static_cast<const void*>(&(*it))))
			status = request_handler->handleReadEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, it->fd);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(*it))))
			status = request_handler->handleWriteEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, it->fd);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(*it))))
			status = request_handler->handleErrorEvent(*io_handler, *conn_manager, it->fd, *timer_tree);

		// kqueueで監視しているイベント情報を更新
		switch (status)
		{
		case RequestHandler::UPDATE_WRITE:
			// keepaliveが無効なので接続を閉じる
			if (!timer_tree->timerExists(it->fd))
				io_handler->closeConnection(*conn_manager, it->fd);
			break;

		default:
			break;
		}
	}
}

/**
 * @brief pollfdのvectorを作成する
 *
 * @param connections : すべてのクライアントソケットとそれにひもづくデータ
 * @return std::vector<struct pollfd>
 */
std::vector<struct pollfd>	PollServer::convertToPollfds(const std::map<int, ConnectionData> &connections)
{
	std::vector<struct pollfd>	list;
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin(); it != connections.end(); ++it)
	{
		struct pollfd	pollfd;
		pollfd.fd = it->first;
		pollfd.events = it->second.event == ConnectionData::EV_READ ? POLLIN : POLLOUT;
		pollfd.revents = 0;
		list.push_back(pollfd);
	}
	return list;
}
