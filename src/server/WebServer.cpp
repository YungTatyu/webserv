#include "WebServer.hpp"

#include <vector>
#include <utility>
#include "LogFd.hpp"

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
	initializeVServers();

	this->requestHandler = new RequestHandler();
	this->connManager = new ConnectionManager();
	initializeConnManager();

	#if defined(KQUEUE_AVAILABLE)
	this->server = new KqueueServer();
	this->eventManager = new KqueueActiveEventManager();
	#else
	this->server = new PollServer();
	this->eventManager = new PollActiveEventManager();
	#endif
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
		// TODO: std::vector<config::Server*>	servers_　に値を格納する関数を呼ぶ
		this->ioHandler->addVServer(listen_fd, TiedServer(address, port));
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
	// close( this->connManager->getConnection() ); // 一応eventLoop()でもクローズしているけど、シグナルで終了した時、逐次処理で行なっているクライアントソケットのクローズが行われていない可能性があるので入れた。
	config::terminateLogFds(this->configHandler->config_);
	delete this->configHandler->config_;
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
