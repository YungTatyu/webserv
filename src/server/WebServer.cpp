#include "WebServer.hpp"

#include <utility>
#include <vector>

#include "LogFd.hpp"

ConfigHandler WebServer::config_handler_;
RequestHandler WebServer::request_handler_;

WebServer::WebServer(const config::Main *config) {
  this->configHandler = &(config_handler_);
  this->configHandler->loadConfiguration(config);
  this->initializeServer();
  if (this->ioHandler->getListenfdMap().size() >=
      this->configHandler->config_->events.worker_connections.getWorkerConnections()) {
    std::stringstream ss;
    ss << "webserv: [emerg] "
       << this->configHandler->config_->events.worker_connections.getWorkerConnections()
       << " worker_connections are not enough for " << this->ioHandler->getListenfdMap().size()
       << " listening sockets";
    this->deleteObjects();
    throw std::runtime_error(ss.str());
  }
}

void WebServer::initializeServer() {
  this->ioHandler = new NetworkIOHandler();
  initializeVServers();

  this->connManager = new ConnectionManager();
  initializeConnManager();

  config::CONNECTION_METHOD method = this->configHandler->config_->events.use.getConnectionMethod();
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
    default:  // kqueueとepoll両方使えない場合は、defaultが必要
      break;
  }
  configHandler->writeErrorLog("webserv: [debug] use " + config::Use::ConnectionMethodToStr(method) + "\n");

  this->timerTree = new TimerTree();
}

void WebServer::initializeListenSocket(std::set<std::pair<std::string, unsigned int> > &ip_address_set,
                                       const std::string address, const unsigned int port) {
  std::pair<std::string, unsigned int> new_pair(address, port);
  if (ip_address_set.find(new_pair) == ip_address_set.end())  // すでに作成したlisten socketは作成しない
  {
    const int listen_fd = this->ioHandler->setupSocket(address, port);
    this->ioHandler->addVServer(listen_fd, configHandler->createTiedServer(address, port));
  }
  ip_address_set.insert(std::make_pair(address, port));
}

void WebServer::initializeVServers() {
  const config::Main *conf = this->configHandler->config_;
  const std::vector<config::Server> &server_list = conf->http.server_list;
  std::set<std::pair<std::string, unsigned int> > ip_address_set;

  for (std::vector<config::Server>::const_iterator sit = server_list.begin(); sit != server_list.end();
       ++sit  // server iterator
  ) {
    // listen directiveが設定されていない時は、default値を設定する
    if (sit->directives_set.find("listen") == sit->directives_set.end()) {
      initializeListenSocket(ip_address_set, config::Listen::kDefaultAddress_, config::Listen::kDefaultPort_);
      continue;
    }

    for (std::vector<config::Listen>::const_iterator lit = sit->listen_list.begin();
         lit != sit->listen_list.end(); ++lit  // listen iterator
    ) {
      initializeListenSocket(ip_address_set, lit->getAddress(), lit->getport());
    }
  }
}

/**
 * @brief listen socketを監視するリストに追加
 *
 */
void WebServer::initializeConnManager() {
  const std::map<int, TiedServer> &listenfd_map = this->ioHandler->getListenfdMap();

  for (std::map<int, TiedServer>::const_iterator it = listenfd_map.begin(); it != listenfd_map.end(); ++it) {
    this->connManager->setConnection(it->first);
    this->connManager->setEvent(it->first, ConnectionData::EV_READ);
  }
}

const ConfigHandler &WebServer::getConfigHandler() { return config_handler_; }

const RequestHandler &WebServer::getRequestHandler() { return request_handler_; }

void WebServer::writeErrorlog(const std::string &msg) { config_handler_.writeErrorLog(msg); }

WebServer::~WebServer() {
  this->configHandler->writeErrorLog("webserv: [debug] Close webserv.\n\n");
  this->deleteObjects();
}

void WebServer::deleteObjects() {
  config::terminateLogFds(this->configHandler->config_);
  delete this->timerTree;
  delete this->ioHandler;
  delete this->connManager;
  delete this->eventManager;
  delete this->server;
}

void WebServer::run() {
  server->eventLoop(this->connManager, this->eventManager, this->ioHandler, this->server, this->timerTree);
}
