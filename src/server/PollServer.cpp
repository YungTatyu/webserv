#include "PollServer.hpp"

PollServer::PollServer() {}

PollServer::~PollServer() {}

void	PollServer::eventLoop(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	for ( ; ; )
	{
		waitForEvent(conn_manager, event_manager);

		// 発生したイベントをhandleする
		callEventHandler(conn_manager, event_manager, io_handler, request_handler);

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
	}
}

int	PollServer::waitForEvent(ConnectionManager*conn_manager, IActiveEventManager *event_manager)
{
	std::vector<pollfd> pollfds = convertToPollfds(conn_manager->getConnections());

	// TODO: error起きたときどうしようか? 一定数retry? serverはdownしたらダメな気がする
	int re = poll(pollfds.data(), pollfds.size(), -1);

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
	RequestHandler* request_handler
)
{
	const std::vector<pollfd> *active_events =
		static_cast<const std::vector<pollfd>*>(event_manager->getActiveEvents());

	// 発生したイベントの数だけloopする
	for (std::vector<pollfd>::const_iterator it = active_events->begin();
		it != active_events->end();
		++it
	)
	{
		// 発生したeventに対するhandlerを呼ぶ
		// interfaceを実装したことにより、関数ポインタのmapが使えなくなった・・・　どうしよう？？？
		if (event_manager->isReadEvent(static_cast<const void*>(&(*it))))
			request_handler->handleReadEvent(*io_handler, *conn_manager, it->fd);
		else if (event_manager->isWriteEvent(static_cast<const void*>(&(*it))))
			request_handler->handleWriteEvent(*io_handler, *conn_manager, it->fd);
		else if (event_manager->isErrorEvent(static_cast<const void*>(&(*it))))
			request_handler->handleErrorEvent(*io_handler, *conn_manager, it->fd);
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
		pollfd.events = it->second.event == ConnectionData::READ ? POLLIN : POLLOUT;
		pollfd.revents = 0;
		list.push_back(pollfd);
	}
	return list;
}
