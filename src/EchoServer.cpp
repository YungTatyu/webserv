#include "EchoServer.hpp"

/* EchoServerクラスの実装 */
EchoServer::EchoServer()
{
	this->serverConfig = new ServerConfig();
	
	this->serverConfig->loadConfiguration(); 
	this->initializeServer();
}

void EchoServer::initializeServer()
{
	this->ioHandler = new NetworkIOHandler();
	this->ioHandler->setupSocket( this->serverConfig );

	this->requestHandler = new RequestHandler();
	this->connManager = new ConnectionManager();
}

void EchoServer::eventLoop()
{
	for ( ; ; )
	{
		this->ioHandler->acceptConnection( *this->connManager );
		for ( ; ; )
		{
			if ( this->ioHandler->receiveData( *this->connManager ) == -1 )
				break ;
			this->requestHandler->handle( *this->connManager );
			this->ioHandler->sendData( *this->connManager );
		};	
		this->ioHandler->closeConnection( *this->connManager );
	};
}

EchoServer::~EchoServer()
{
	close( this->listenfd ); // リスニングソケットのクローズ 
	delete this->ioHandler;
	delete this->requestHandler;
	delete this->connManager;
	delete this->serverConfig;
}

