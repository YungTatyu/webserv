#ifndef NETWORK_IO_HANDLER_HPP
#define NETWORK_IO_HANDLER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Listen.hpp"
#include "Server.hpp"
#include "SysCallWrapper.hpp"
#include "TimerTree.hpp"

class ConfigHandler;
class ConnectionManager;
class IServer;

/* listen socketと結びついたserver config を持つ構造体 */
struct TiedServer {
  std::vector<const config::Server*> servers_;
  const std::string addr_;
  const unsigned int port_;

  TiedServer() : addr_(config::Listen::kDefaultAddress_), port_(config::Listen::kDefaultPort_){};
  TiedServer(const std::string& addr, unsigned int port) : addr_(addr), port_(port) {}
  TiedServer(const TiedServer& other) :servers_(other.servers_), addr_(other.addr_), port_(other.port_)  {}
  TiedServer& operator=(const TiedServer& other) {
    if (this != &other) {
      this->servers_ = other.servers_;
      // this->addr_ = other.addr_;
      // this->port_ = other.port_;
    }
    return *this;
  }
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler {
 public:
  NetworkIOHandler();
  ~NetworkIOHandler();
  int setupSocket(const std::string& address, unsigned int port);
  ssize_t receiveRequest(ConnectionManager& connManager, int sock);
  ssize_t sendResponse(ConnectionManager& connManager, int sock);
  ssize_t receiveCgiResponse(ConnectionManager& connManager, int sock);
  ssize_t sendRequestBody(ConnectionManager& connManager,  int sock);
  int acceptConnection(ConnectionManager& connManager, int listen_fd);
  void closeConnection(ConnectionManager& connManager, IServer* server, TimerTree& timerTree,  int sock);
  void purgeConnection(ConnectionManager& connManager, IServer* server, TimerTree& timerTree,  int sock);
  void closeAllListenSockets();
  const std::map<int, TiedServer>& getListenfdMap();
  void addVServer( int listen_fd, const TiedServer& server);
  bool isListenSocket( int listen_fd) const;
  static size_t getBufferSize();

 private:
  std::map<int, TiedServer> listenfd_map_;  // リスニングソケットとそれに紐づくserver configを管理
  const static size_t buffer_size_ = 1024;
};

#endif
