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
	this->eventManager->addEvent( EventManager::genPollFd( this->ioHandler->getListenfd(), POLLIN, 0 ) );

	for ( ; ; )
	{
		SysCallWrapper::Poll ( this->eventManager->fds.data(), this->eventManager->fds.size(), -1 );

		//　ここをイテレータで走査したら、要素を追加したときにイテレータが無効になったりしてバグる。
		size_t iniSize = this->eventManager->fds.size();
		for ( size_t i = 0; i < iniSize; ++i )
		{
			struct pollfd& curPfd = this->eventManager->fds[i];	
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
					else if ( re == -1 && errno == EAGAIN ) //ソケット使用不可。
					{
						continue ;
					}	
					this->requestHandler->handle( *this->connManager, curPfd.fd );
					this->eventManager->updateEvents( curPfd.fd , POLLOUT );
				}
			}
			else if ( curPfd.revents & POLLOUT ) //レスポンス送信
			{
				if ( this->ioHandler->sendResponse( *this->connManager, curPfd.fd ) == -1 && errno == EAGAIN )
					continue ;
				this->eventManager->updateEvents( curPfd.fd , POLLIN );
			}
		}

		this->eventManager->fds.erase(
			std::remove_if( this->eventManager->fds.begin(), this->eventManager->fds.end(), EventManager::isInvalidFd ),
			this->eventManager->fds.end()
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

