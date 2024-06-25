#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include "ConfigHandler.hpp"
#include "ConnectionManager.hpp"
#include "EpollActiveEventManager.hpp"
#include "EpollServer.hpp"
#include "IActiveEventManager.hpp"
#include "IServer.hpp"
#include "KqueueActiveEventManager.hpp"
#include "KqueueServer.hpp"
#include "NetworkIOHandler.hpp"
#include "PollActiveEventManager.hpp"
#include "PollServer.hpp"
#include "RequestHandler.hpp"
#include "SelectActiveEventManager.hpp"
#include "SelectServer.hpp"
#include "syscall_wrapper.hpp"

class WebServer {
 public:
  WebServer(const config::Main *config);
  ~WebServer();
  void run();
  const static ConfigHandler &getConfigHandler();
  const static RequestHandler &getRequestHandler();
  static void writeErrorlog(const std::string &msg);

 private:
  // メンバ変数を持たない、もしくは基本的に値を変更しないオブジェクトはstatic変数で管理する
  static ConfigHandler config_handler_;
  static RequestHandler request_handler_;
  IServer *server_;
  NetworkIOHandler *io_handler_;
  ConnectionManager *conn_manager_;
  IActiveEventManager *event_manager_;
  TimerTree *timer_tree_;
  void initializeServer();
  void initializeVServers();
  void initializeListenSocket(std::set<std::pair<std::string, unsigned int> > &ip_address_set,
                              const std::string &address, unsigned int port);
  void initializeConnManager();
  void deleteObjects();
};

#endif
