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

void WebServer::eventLoop()
{
	for ( ; ; )
	{
		this->ioHandler->acceptConnection( *this->connManager );
		for ( ; ; )
		{
			if ( this->ioHandler->receiveRequest( *this->connManager ) == -1 )
				break ;
			this->requestHandler->handle( *this->connManager );
			this->ioHandler->sendResponse( *this->connManager );
		};	
		this->ioHandler->closeConnection( *this->connManager );
	};
}

WebServer::~WebServer()
{
	close( this->listenfd ); // リスニングソケットのクローズ 
	delete this->ioHandler;
	delete this->requestHandler;
	delete this->connManager;
	delete this->serverConfig;
}

