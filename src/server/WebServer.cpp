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
		processEvents(pollfds);

		// 発生したすべてイベントを削除
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
		if ((cur_pfd.revents & POLLIN) || (cur_pfd.revents & POLLOUT))
			this->eventManager->addEvent(cur_pfd);
	}
}

void	WebServer::processEvents(const std::vector<struct pollfd> &pollfds)
{
	for (std::vector<struct pollfd>::const_iterator it = pollfds.begin();
		it != pollfds.end();
		++it
	)
	{
		callEventHandler(*it);
		// {
		// 	{READ, isReadEvent()},
		// 	{WRITE, isWriteEvent()},
		// }
		// EVENT = findEvent();
		// handler = handler_map[EVENT];
		// handler();
		// if  read
			// request_handler.readEventHandler();
		// else if  write
			// request_handler.writeEventHandler();
	}
}

/**
 * @brief 発生したイベントのhandlerを呼ぶ
 * eventhandlerを呼んだ後、監視するイベントを更新
 * 
 * @param pollfd 
 */
void	WebServer::callEventHandler(const struct pollfd &pollfd)
{
	if (pollfd.revents & POLLIN) // read event
	{
		this->requestHandler->handleReadEvent(*(this->ioHandler), *(this->connManager), pollfd);
	}
	else if (pollfd.revents & POLLOUT) // write event
	{
		this->requestHandler->handleWriteEvent(*(this->ioHandler), *(this->connManager), pollfd);
	}
}
