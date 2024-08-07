#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include "ConfigHandler.hpp"
#include "ConnectionManager.hpp"
#include "EpollActiveEventManager.hpp"
#include "EpollServer.hpp"
#include "EventHandler.hpp"
#include "IActiveEventManager.hpp"
#include "IServer.hpp"
#include "KqueueActiveEventManager.hpp"
#include "KqueueServer.hpp"
#include "NetworkIOHandler.hpp"
#include "PollActiveEventManager.hpp"
#include "PollServer.hpp"
#include "SelectActiveEventManager.hpp"
#include "SelectServer.hpp"
#include "syscall_wrapper.hpp"

class WebServer {
 public:
  WebServer(const config::Main *config);
  ~WebServer();
  void run();
  static const ConfigHandler &getConfigHandler();
  static const EventHandler &getEventHandler();
  static void writeErrorlog(const std::string &msg, config::LOG_LEVEL level);

 private:
  WebServer();
  WebServer(const WebServer &);
  WebServer &operator=(const WebServer &);
  void initializeServer();
  void initializeVServers();
  void initializeListenSocket(std::set<std::pair<std::string, unsigned int> > &ip_address_set,
                              const std::string &address, unsigned int port);
  void initializeConnManager();
  void deleteObjects();

  // メンバ変数を持たない、もしくは基本的に値を変更しないオブジェクトはstatic変数で管理する
  static ConfigHandler config_handler_;
  static EventHandler event_handler_;
  IServer *server_;
  NetworkIOHandler *io_handler_;
  ConnectionManager *conn_manager_;
  IActiveEventManager *event_manager_;
  TimerTree *timer_tree_;
};

#endif
