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

	#if defined(KQUEUE_AVAILABLE)
	this->server = new KqueueServer();
	this->eventManager = new KqueueActiveEventManager();
	#else
	this->server = new PollServer();
	this->eventManager = new PollActiveEventManager();
	#endif

	// listening socketを監視するリストに追加
	const int listenfd = this->ioHandler->getListenfd();
	this->connManager->setConnection(listenfd);
	this->connManager->setEvent(listenfd, ConnectionData::READ);
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

void	WebServer::run()
{
	this->server->eventLoop(this->connManager, this->eventManager, this->ioHandler, this->requestHandler);
}
