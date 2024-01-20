#include "WebServer.hpp"
#include "EventManager.hpp"
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
//
// #include <iostream>
// void printpfdvector( std::vector<struct pollfd> src, int level )
// {
// 	std::cout << level << "-----------------" << std::endl;
// 	std::cout << "num of vec size: " << src.size() << std::endl;
// 	int i = 0;
// 	for ( std::vector<struct pollfd>::iterator src_it = src.begin(); src_it != src.end(); ++src_it )
// 	{
// 		std::cout << "element----" << i << std::endl;
// 		std::cout << "fd: " << src_it->fd << std::endl;
// 		std::cout << "events: " << src_it->events << std::endl;
// 		std::cout << "revents: " << src_it->revents << std::endl;
// 		++i;
// 	}
// 	std::cout << "-----------------" << std::endl;
// }

bool isFdNegative(const struct pollfd& pfd)
{
	return pfd.fd == -1;
}

void WebServer::eventLoop()
{
	this->eventManager->fds.push_back( EventManager::genPollFd( this->ioHandler->getListenfd(), POLLIN, 0 ) );

	for ( ; ; )
	{
		poll ( this->eventManager->fds.data(), this->eventManager->fds.size(), -1 );

		//　ここをイテレータで走査したら、要素を追加したときにイテレータが無効になったりしてバグる。
		size_t iniSize = this->eventManager->fds.size();
		for ( size_t i = 0; i < iniSize; ++i )
		{
			struct pollfd &curPfd = this->eventManager->fds[i];	
			if ( curPfd.revents & POLLIN )
			{
				if ( curPfd.fd == this->ioHandler->getListenfd() )	
				{
					this->ioHandler->acceptConnection( *this->connManager, *this->eventManager );
				}
				else
				{
					int re = this->ioHandler->receiveRequest( *this->connManager, curPfd.fd );
					if ( re == 0 )
					{
						this->ioHandler->closeConnection( *this->connManager, curPfd.fd );
						curPfd.fd = -1;
						continue ;
					}
					else if ( re == -1 && errno == EAGAIN )
					{
						continue ;
					}	
					this->requestHandler->handle( *this->connManager, curPfd.fd );
					this->eventManager->updateEvents( curPfd.fd , POLLOUT );
				}
			}
			else if ( curPfd.revents & POLLOUT )
			{
				if ( this->ioHandler->sendResponse( *this->connManager, curPfd.fd ) == -1 && errno == EAGAIN )
					continue ;
				this->eventManager->updateEvents( curPfd.fd , POLLIN );
			}
		}

		this->eventManager->fds.erase(
			std::remove_if( this->eventManager->fds.begin(), this->eventManager->fds.end(), isFdNegative ),
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

