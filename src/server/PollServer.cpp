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
		callEventHandler(event_manager, io_handler, request_handler);

		// 発生したすべてのイベントを削除
		event_manager->clearAllEvents();
	}
}

int	PollServer::waitForEvent(ConnectionManager* conn_manager)
{
	std::vector<struct pollfd> pollfds = convertToPollfds(conn_manager->getConnections());

	int re = SysCallWrapper::Poll(pollfds.data(), pollfds.size(), -1);

	// 発生したイベントをActiveEventManagerにすべて追加
	addActiveEvents(pollfds);
	return re;
}

void	PollServer::callEventHandler(
	IActiveEventManager* event_manager,
	NetworkIOHandler* io_handler,
	RequestHandler* request_handler
)
{

}