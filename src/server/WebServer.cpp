#include "WebServer.hpp"

#include <utility>
#include <vector>

#include "HttpResponse.hpp"
#include "LogFd.hpp"

ConfigHandler WebServer::config_handler_;
RequestHandler WebServer::request_handler_;

WebServer::WebServer(const config::Main *config) {
  config_handler_.loadConfiguration(config);
  this->initializeServer();
  if (this->io_handler_->getListenfdMap().size() >=
      config_handler_.config_->events_.worker_connections_.getWorkerConnections()) {
    std::stringstream ss;
    ss << "webserv: [emerg] " << config_handler_.config_->events_.worker_connections_.getWorkerConnections()
       << " worker_connections are not enough for " << this->io_handler_->getListenfdMap().size()
       << " listening sockets";
    this->deleteObjects();
    throw std::runtime_error(ss.str());
  }
}

void WebServer::initializeServer() {
  this->io_handler_ = new NetworkIOHandler();
  initializeVServers();
  this->conn_manager_ = new ConnectionManager();
  initializeConnManager();
  config::CONNECTION_METHOD method = config_handler_.config_->events_.use_.getConnectionMethod();
  switch (method) {
#if defined(KQUEUE_AVAILABLE)
    case config::KQUEUE:
      this->server_ = new KqueueServer();
      this->event_manager_ = new KqueueActiveEventManager();
      break;
#endif
#if defined(EPOLL_AVAILABLE)
    case config::EPOLL:
      this->server_ = new EpollServer();
      this->event_manager_ = new EpollActiveEventManager();
      break;
#endif
    case config::POLL:
      this->server_ = new PollServer();
      this->event_manager_ = new PollActiveEventManager();
      break;
    case config::SELECT:
      this->server_ = new SelectServer();
      this->event_manager_ = new SelectActiveEventManager();
      break;
    default:  // kqueueとepoll両方使えない場合は、defaultが必要
      break;
  }
  config_handler_.writeErrorLog("webserv: [notice] use_ " + config::Use::ConnectionMethodToStr(method) + "\n",
                                config::NOTICE);
  this->timer_tree_ = new TimerTree();
  HttpResponse::setup();
}

void WebServer::initializeListenSocket(std::set<std::pair<std::string, unsigned int> > &ip_address_set,
                                       const std::string &address, unsigned int port) {
  std::pair<std::string, unsigned int> new_pair(address, port);
  if (ip_address_set.find(new_pair) == ip_address_set.end())  // すでに作成したlisten socketは作成しない
  {
    const int listen_fd = this->io_handler_->setupSocket(address, port);
    this->io_handler_->addVServer(listen_fd, config_handler_.createTiedServer(address, port));
  }
  ip_address_set.insert(std::make_pair(address, port));
}

void WebServer::initializeVServers() {
  const config::Main *conf = config_handler_.config_;
  const std::vector<config::Server> &server_list = conf->http_.server_list_;
  std::set<std::pair<std::string, unsigned int> > ip_address_set;

  for (std::vector<config::Server>::const_iterator sit = server_list.begin(); sit != server_list.end();
       ++sit  // server iterator
  ) {
    // listen directiveが設定されていない時は、default値を設定する
    if (sit->directives_set_.find("listen") == sit->directives_set_.end()) {
      initializeListenSocket(ip_address_set, config::Listen::kDefaultAddress_, config::Listen::kDefaultPort_);
      continue;
    }

    for (std::vector<config::Listen>::const_iterator lit = sit->listen_list_.begin();
         lit != sit->listen_list_.end(); ++lit  // listen iterator
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
  const std::map<int, TiedServer> &listenfd_map = this->io_handler_->getListenfdMap();

  for (std::map<int, TiedServer>::const_iterator it = listenfd_map.begin(); it != listenfd_map.end(); ++it) {
    this->conn_manager_->setConnection(it->first);
    this->conn_manager_->setEvent(it->first, ConnectionData::EV_READ);
  }
}

const ConfigHandler &WebServer::getConfigHandler() { return config_handler_; }

const RequestHandler &WebServer::getRequestHandler() { return request_handler_; }

void WebServer::writeErrorlog(const std::string &msg, config::LOG_LEVEL level) {
  config_handler_.writeErrorLog(msg, level);
}

WebServer::~WebServer() {
  config_handler_.writeErrorLog("webserv: [debug] Close webserv.\n\n", config::DEBUG);
  this->deleteObjects();
}

void WebServer::deleteObjects() {
  config::terminateLogFds(config_handler_.config_);
  delete this->timer_tree_;
  delete this->io_handler_;
  delete this->conn_manager_;
  delete this->event_manager_;
  delete this->server_;
}

void WebServer::run() {
  this->server_->eventLoop(this->conn_manager_, this->event_manager_, this->io_handler_, this->timer_tree_);
}
