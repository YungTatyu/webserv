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
		waitForEvent(conn_manager);

		// 発生したイベントをhandleする
		callEventHandler(conn_manager, event_manager, io_handler, request_handler);

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
	}
}

int	PollServer::waitForEvent(ConnectionManager* conn_manager)
{
	std::vector<struct pollfd> pollfds = convertToPollfds(conn_manager->getConnections());

	// TODO: error起きたときどうしようか? 一定数retry? serverはdownしたらダメな気がする
	int re = SysCallWrapper::Poll(pollfds.data(), pollfds.size(), -1);

	// 発生したイベントをActiveEventManagerにすべて追加
	addActiveEvents(pollfds);
	return re;
}

void	PollServer::callEventHandler(
	ConnectionManager* conn_manager,
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{
	const std::vector<struct pollfd> *active_events =
		static_cast<const std::vector<struct pollfd>*>(event_manager->getActiveEvents());
	const std::map<RequestHandler::whichEvent, RequestHandler::eventHandler>	&handler_map =
		request_handler->handler_map;

	// 発生したイベントの数だけloopする
	// eit: event iterator
	for (std::vector<struct pollfd>::const_iterator eit = active_events->begin();
		eit != active_events->end();
		++eit
	)
	{
		// 発生したeventに対するhandlerを呼ぶ
		// mit: map iterator
		for (
			std::map<RequestHandler::whichEvent, RequestHandler::eventHandler>::const_iterator mit = handler_map.begin();
			mit != handler_map.end();
			++mit
		)
		{
			if ((*mit->first)(*eit))
			{
				RequestHandler::eventHandler event_handler = mit->second;
				(request_handler->*event_handler)(*(io_handler), *(conn_manager), eit->fd);
				break;
			}
		}
	}

}