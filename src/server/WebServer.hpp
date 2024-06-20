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
#include "SysCallWrapper.hpp"

class WebServer {
 public:
  WebServer(const config::Main *config);
  ~WebServer();
  void run();
  static const ConfigHandler &getConfigHandler();
  static void writeErrorlog(const std::string &msg);
  static IServer *getServer();

 private:
  // メンバ変数を持たない、もしくは基本的に値を変更しないオブジェクトはstatic変数で管理する
  // TODO: 値を固定するオブジェクトはstaticに合わせたい、（関数の引数で渡す必要がない）
  // 引数が多くなりすぎるから
  static ConfigHandler config_handler_;
  static IServer *server;
  NetworkIOHandler *ioHandler;
  RequestHandler *requestHandler;
  ConnectionManager *connManager;
  IActiveEventManager *eventManager;
  ConfigHandler *configHandler;
  TimerTree *timerTree;
  void initializeServer();
  void initializeVServers();
  void initializeListenSocket(std::set<std::pair<std::string, unsigned int> > &ip_address_set,
                              const std::string address, const unsigned int port);
  void initializeConnManager();
  void deleteObjects();
};

#endif
