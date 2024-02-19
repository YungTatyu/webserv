#include "WebServer.hpp"
#include "ActiveEventManager.hpp"
#include "SysCallWrapper.hpp"
#include <cerrno>

/* WebServerクラスの実装 */
WebServer::WebServer()
{
	this->serverConfig = new ServerConfig();

	this->serverConfig->loadConfiguration();
	this->initializeServer();
}

void WebServer::initializeServer()
{
	this->ioHandler = new NetworkIOHandler();
	this->ioHandler->setupSocket( this->serverConfig );

	this->requestHandler = new RequestHandler();
	this->connManager = new ConnectionManager();
	this->eventManager = new ActiveEventManager();
}

void WebServer::eventLoop()
{
	// listening socketを監視するリストに追加
	const int listenfd = this->ioHandler->getListenfd();
	this->connManager->setConnection(listenfd);
	this->connManager->setEvent(listenfd, ConnectionData::READ);

	for ( ; ; )
	{
		std::vector<struct pollfd> pollfds = convertToPollfds(this->connManager->getConnections());
		waitForEvents(pollfds);

		// 発生したイベントをActiveEventManagerにすべて追加
		addActiveEvents(pollfds);

		// 発生したイベントをhandleする
		callEventHandler();

		// 発生したすべてのイベントを削除
		this->eventManager->clearAllEvents();
	}
}

WebServer::~WebServer()
{
	close( this->ioHandler->getListenfd() ); // リスニングソケットのクローズ
	// close( this->connManager->getConnection() ); // 一応eventLoop()でもクローズしているけど、シグナルで終了した時、逐次処理で行なっているクライアントソケットのクローズが行われていない可能性があるので入れた。
	delete this->ioHandler;
	delete this->requestHandler;
	delete this->connManager;
	delete this->serverConfig;
	delete this->eventManager;
}

/**
 * @brief pollfdのvectorを作成する
 *
 * @param connections : すべてのクライアントソケットとそれにひもづくデータ
 * @return std::vector<struct pollfd>
 */
std::vector<struct pollfd>	WebServer::convertToPollfds(const std::map<int, ConnectionData> &connections)
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

int	WebServer::waitForEvents(std::vector<struct pollfd> &pollfds)
{
	 return SysCallWrapper::Poll ( pollfds.data(), pollfds.size(), -1 );
}

void	WebServer::addActiveEvents(const std::vector<struct pollfd> &pollfds)
{
	const size_t	size = this->connManager->getConnections().size();
	for (size_t i = 0; i < size; ++i)
	{
		const struct pollfd& cur_pfd = pollfds[i];
		// readもしくはwriteイベントが発生していたら、active_eventに追加
		if (ActiveEventManager::isReadEvent(cur_pfd) || ActiveEventManager::isWriteEvent(cur_pfd))
			this->eventManager->addEvent(cur_pfd);
	}
}

/**
 * @brief 発生したイベントのhandlerを呼ぶ
 * eventhandlerを呼んだ後、監視するイベントを更新
 * 
 */
void	WebServer::callEventHandler()
{
	const std::vector<struct pollfd> &active_events = this->eventManager->active_events_;
	const std::map<RequestHandler::whichEvent, RequestHandler::eventHandler>	&handler_map = this->requestHandler->handler_map;

	// 発生したイベントの数だけloopする
	// eit: event iterator
	for (std::vector<struct pollfd>::const_iterator eit = active_events.begin();
		eit != active_events.end();
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
				(this->requestHandler->*event_handler)(*(this->ioHandler), *(this->connManager), eit->fd);
				break;
			}
		}
	}
}
