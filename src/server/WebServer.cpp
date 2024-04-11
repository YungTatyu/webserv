#include "WebServer.hpp"
#include "LogFd.hpp"

#include <vector>
#include <utility>

/* WebServerクラスの実装 */
WebServer::WebServer( const config::Main* config )
{
	this->configHandler = new ConfigHandler();

	this->configHandler->loadConfiguration(config);
	this->initializeServer();
	if (this->ioHandler->getListenfdMap().size() >= this->configHandler->config_->events.worker_connections.getWorkerConnections())
	{
		std::cout << "webserv: [emerg] " << this->configHandler->config_->events.worker_connections.getWorkerConnections() << " worker_connections are not enough for " << this->ioHandler->getListenfdMap().size() << " listening sockets" << std::endl;
		delete this->timerTree;
		config::terminateLogFds(this->configHandler->config_);
		delete this->configHandler->config_;
		delete this->configHandler;
		delete this->ioHandler;
		delete this->requestHandler;
		delete this->connManager;
		delete this->eventManager;
		delete this->server;
		throw std::exception();
	}
}

void WebServer::initializeServer()
{
	this->ioHandler = new NetworkIOHandler();
	initializeVServers();

	this->requestHandler = new RequestHandler();
	this->connManager = new ConnectionManager();
	initializeConnManager();

	config::CONNECTION_METHOD	method = this->configHandler->config_->events.use.getConnectionMethod();
	switch (method) {
		#if defined(KQUEUE_AVAILABLE)
		case config::KQUEUE:
			this->server = new KqueueServer();
			this->eventManager = new KqueueActiveEventManager();
			break;
		case config::EPOLL:
			break;
		#endif
		#if defined(EPOLL_AVAILABLE)
		case config::KQUEUE:
			break;
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
	}
	configHandler->writeErrorLog("webserv: [debug] use " + config::Use::ConnectionMethodToStr(method) + "\n");

	this->timerTree = new TimerTree();

}

void	WebServer::initializeListenSocket(
	std::set<std::pair<std::string, unsigned int> > &ip_address_set,
	const std::string address,
	const unsigned int port
)
{
	std::pair<std::string, unsigned int>	new_pair(address, port); 
	if (ip_address_set.find(new_pair) == ip_address_set.end()) // すでに作成したlisten socketは作成しない
	{
		const int listen_fd = this->ioHandler->setupSocket(address, port);
		this->ioHandler->addVServer(listen_fd, configHandler->createTiedServer(address, port));
	}
	ip_address_set.insert(std::make_pair(address, port));
}

void	WebServer::initializeVServers()
{
	const config::Main	*conf = this->configHandler->config_;
	const std::vector<config::Server>	&server_list = conf->http.server_list;
	std::set<std::pair<std::string, unsigned int> > ip_address_set;

	for (std::vector<config::Server>::const_iterator sit = server_list.begin();
		sit != server_list.end();
		++sit // server iterator
	)
	{
		// listen directiveが設定されていない時は、default値を設定する
		if (sit->directives_set.find("listen") == sit->directives_set.end())
		{
			initializeListenSocket(ip_address_set, config::Listen::kDefaultAddress_, config::Listen::kDefaultPort_);
			continue;
		}

		for (std::vector<config::Listen>::const_iterator lit = sit->listen_list.begin();
			lit != sit->listen_list.end();
			++lit // listen iterator
		)
		{
			initializeListenSocket(ip_address_set, lit->getAddress(), lit->getport());
		}
	}
}

/**
 * @brief listen socketを監視するリストに追加
 * 
 */
void	WebServer::initializeConnManager()
{
	const std::map<int, TiedServer>&	listenfd_map = this->ioHandler->getListenfdMap();

	for (std::map<int, TiedServer>::const_iterator it = listenfd_map.begin();
		it != listenfd_map.end();
		++it
	)
	{
		this->connManager->setConnection(it->first);
		this->connManager->setEvent(it->first, ConnectionData::EV_READ);
	}
}

WebServer::~WebServer()
{
	this->configHandler->writeErrorLog("webserv: [debug] Close webserv.\n\n");
	// close( this->connManager->getConnection() ); // 一応eventLoop()でもクローズしているけど、シグナルで終了した時、逐次処理で行なっているクライアントソケットのクローズが行われていない可能性があるので入れた。
	config::terminateLogFds(this->configHandler->config_);
	delete this->timerTree;
	delete this->configHandler->config_;
	delete this->configHandler;
	delete this->ioHandler;
	delete this->requestHandler;
	delete this->connManager;
	delete this->eventManager;
	delete this->server;
}

void	WebServer::run()
{
	this->server->eventLoop(this->connManager, this->eventManager, this->ioHandler, this->requestHandler, this->configHandler, this->timerTree);
}
