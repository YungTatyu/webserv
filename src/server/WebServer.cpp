#include "WebServer.hpp"

/* WebServerクラスの実装 */
WebServer::WebServer( const config::Main* config )
{
	this->configHandler = new ConfigHandler();

	this->configHandler->loadConfiguration(config);
	this->initializeServer();
}

void WebServer::initializeServer()
{
	this->ioHandler = new NetworkIOHandler();
	this->ioHandler->setupSocket( this->configHandler );

	this->requestHandler = new RequestHandler();
	this->connManager = new ConnectionManager();

	config::CONNECTION_METHOD	method = this->configHandler->config_->events.use.getConnectionMethod();
	switch (method) {
		#if defined(KQUEUE_AVAILABLE)
		case config::KQUEUE:
			this->server = new KqueueServer();
			this->eventManager = new KqueueActiveEventManager();
			break;
		#endif
		#if defined(EPOLL_AVAILABLE)
		case config::EPOLL:
			this->server = new EpollServer();
			this->eventManager = new EpollActiveEventManager();
			break;
		#endif
		case config::POLL:
			this->server = new PollServer();
			this->eventManager = new PollActiveEventManager();
			break;
		case config::SELECT:
			this->server = new SelectServer();
			this->eventManager = new SelectActiveEventManager();
			break;
		default:
			this->server = new PollServer();
			this->eventManager = new PollActiveEventManager();
			break;
	}

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
	delete this->configHandler;
	delete this->eventManager;
}

void	WebServer::run()
{
	this->server->eventLoop(this->connManager, this->eventManager, this->ioHandler, this->requestHandler);
}
