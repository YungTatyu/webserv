#include "WebServer.hpp"
#include "EventManager.hpp"
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
	this->eventManager = new EventManager();
}

void WebServer::eventLoop()
{
	// listening socket監視するリストに追加
	const int listenfd = this->ioHandler->getListenfd();
	this->connManager->setConnection(listenfd);
	this->connManager->setEvent(listenfd, ConnectionData::READ);

	for ( ; ; )
	{
		std::vector<struct pollfd> pollfds = convertToPollfds(this->connManager->getConnections());
		
		SysCallWrapper::Poll ( pollfds.data(), pollfds.size(), -1 );
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
	std::vector<struct pollfd>	pollfds;
	for (std::map<int, ConnectionData>::const_iterator it = connections.begin(); it != connections.end(); ++it)
	{
		struct pollfd	pollfd;
		pollfd.fd = it->first;
		pollfd.events = it->second.event == ConnectionData::READ ? POLLIN : POLLOUT;
		pollfd.revents = 0;
		pollfds.push_back(pollfd);
	}
	return pollfds;
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
		ssize_t re = this->ioHandler->receiveRequest( *this->connManager, pollfd.fd );
		// リスニングソケットへの新規リクエスト
		if (pollfd.fd == this->ioHandler->getListenfd())
		{
			this->ioHandler->acceptConnection(*(this->connManager));
			return;
		}
		// クライアントソケットへのリクエスト（既存コネクション）
		if (re == -1) //ソケット使用不可。
			return;
		if (re == 0) // クライアントが接続を閉じる
		{
			this->ioHandler->closeConnection( *this->connManager, pollfd.fd );
			this->connManager->removeConnection(pollfd.fd);
			return;
		}
		this->requestHandler->handle( *this->connManager, pollfd.fd );
		this->connManager->setEvent(pollfd.fd, ConnectionData::WRITE); // writeイベントに更新
	}
	else if (pollfd.revents & POLLOUT) // write event
	{
		if (this->ioHandler->sendResponse( *this->connManager, pollfd.fd ) != -1)
			this->connManager->setEvent(pollfd.fd, ConnectionData::READ); // readイベントに更新
	}
}
