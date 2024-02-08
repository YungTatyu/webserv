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
	// this->eventManager->addEvent( EventManager::genPollFd( this->ioHandler->getListenfd(), POLLIN, 0 ) );
	// listening socket監視するリストに追加
	const int listenfd = this->ioHandler->getListenfd();
	this->connManager->setConnection(listenfd);
	this->connManager->setEvent(listenfd, ConnectionData::READ);

	for ( ; ; )
	{
		// SysCallWrapper::Poll ( this->eventManager->active_events_.data(), this->eventManager->active_events_.size(), -1 );
		std::vector<struct pollfd> pollfds = convertToPollfds(this->connManager->getConnections());
		SysCallWrapper::Poll ( pollfds.data(), pollfds.size(), -1 );

		//　ここをイテレータで走査したら、要素を追加したときにイテレータが無効になったりしてバグる。
		size_t iniSize = this->connManager->getConnections().size();
		for ( size_t i = 0; i < iniSize; ++i )
		{
			struct pollfd& curPfd = this->eventManager->active_events_[i];
			if ( curPfd.revents & POLLIN ) //リクエスト受信
			{
				if ( curPfd.fd == this->ioHandler->getListenfd() ) // リスニングソケットへの新規リクエスト。(新規コネクション)
				{
					this->ioHandler->acceptConnection( *this->connManager, *this->eventManager );
				}
				else // クライアントソケットへのリクエスト。(既存コネクション）
				{
					int re = this->ioHandler->receiveRequest( *this->connManager, curPfd.fd );
					if ( re == 0 ) //クライアントが接続を閉じる時。
					{
						this->ioHandler->closeConnection( *this->connManager, curPfd.fd );
						curPfd.fd = -1;
						continue ;
					}
					else if ( re == -1 ) //ソケット使用不可。
					{
						continue ;
					}
					this->requestHandler->handle( *this->connManager, curPfd.fd );
					this->eventManager->updateEvents( curPfd.fd , POLLOUT );
				}
			}
			else if ( curPfd.revents & POLLOUT ) //レスポンス送信
			{
				if ( this->ioHandler->sendResponse( *this->connManager, curPfd.fd ) == -1 )
					continue ;
				this->eventManager->updateEvents( curPfd.fd , POLLIN );
			}
		}

		this->eventManager->active_events_.erase(
			std::remove_if( this->eventManager->active_events_.begin(), this->eventManager->active_events_.end(), EventManager::isInvalidFd ),
			this->eventManager->active_events_.end()
		);
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
		if (cur_pfd.revents & POLLIN || cur_pfd.revents & POLLOUT)
			this->eventManager.addEvent(cur_pfd);
	}
}

void	WebServer::processEvents()
{

}

void	WebServer::callEventHandler(const struct pollfd pollfd)
{
	if (pollfd.revents & POLLIN)
	{
		ssize_t re = this->ioHandler->receiveRequest( *this->connManager, pollfd.fd );
		if (re == -1) //ソケット使用不可。
			return;
		if (re == 0)
		{
			this->ioHandler->closeConnection( *this->connManager, pollfd.fd );
			this->connManager.removeConnection(pollfd.fd);
			return;
		}
		this->requestHandler->handle( *this->connManager, pollfd.fd );
	}
	else if (pollfd.revents & POLLOUT)
	{
		this->ioHandler->sendResponse( *this->connManager, curPfd.fd );
	}
}
