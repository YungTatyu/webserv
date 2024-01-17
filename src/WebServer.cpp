#include "WebServer.hpp"

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
}

#include <iostream>
void printpfdvector( std::vector<struct pollfd> src, int level )
{
	std::cout << level << "-----------------" << std::endl;
	std::cout << "num of vec size: " << src.size() << std::endl;
	int i = 0;
	for ( std::vector<struct pollfd>::iterator src_it = src.begin(); src_it != src.end(); ++src_it )
	{
		std::cout << "element----" << i << std::endl;
		std::cout << "fd: " << src_it->fd << std::endl;
		std::cout << "events: " << src_it->events << std::endl;
		std::cout << "revents: " << src_it->revents << std::endl;
		++i;
	}
	std::cout << "-----------------" << std::endl;
}

bool isFdNegative(const struct pollfd& pfd)
{
	return pfd.fd == -1;
}

void WebServer::eventLoop()
{
	struct pollfd tmppollfd;
	tmppollfd.fd = this->ioHandler->getListenfd();
	tmppollfd.events = POLLIN;
	tmppollfd.revents = 0;
	this->connManager->fds.push_back( tmppollfd );
	// printpfdvector(this->connManager->fds, 0);

	for ( ; ; )
	{
		// printpfdvector(this->connManager->fds, 1);
		poll ( this->connManager->fds.data(), this->connManager->fds.size(), -1 );

		//　ここをイテレータで走査したら、要素を追加したときにイテレータが無効になったりしてバグる。
		size_t tmpsize = this->connManager->fds.size();
		for ( size_t i = 0; i < tmpsize; ++i )
		{
			// printpfdvector(this->connManager->fds, 2);

			if ( this->connManager->fds[i].revents & POLLIN )
			{
				if ( this->connManager->fds[i].fd == this->ioHandler->getListenfd() )	
				{
					this->ioHandler->acceptConnection( *this->connManager );
				}
				else
				{
					if ( this->ioHandler->receiveRequest( *this->connManager, this->connManager->fds[i].fd ) == -1 )
					{
						this->ioHandler->closeConnection( *this->connManager, this->connManager->fds[i].fd );
						this->connManager->fds[i].fd = -1;
						continue ;
					}
					this->requestHandler->handle( *this->connManager, this->connManager->fds[i].fd );
					this->ioHandler->sendResponse( *this->connManager, this->connManager->fds[i].fd  );
				}
			}
		}
		this->connManager->fds.erase(
			std::remove_if( this->connManager->fds.begin(), this->connManager->fds.end(), isFdNegative ),
			this->connManager->fds.end()
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
}

