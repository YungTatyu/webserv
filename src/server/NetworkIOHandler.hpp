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
#include "TimerTree.hpp"
#include "syscall_wrapper.hpp"

class ConfigHandler;
class ConnectionManager;
class IServer;

/* listen socketと結びついたserver config を持つ構造体 */
struct TiedServer {
  TiedServer() : addr_(config::Listen::kDefaultAddress_), port_(config::Listen::kDefaultPort_){};
  TiedServer(const std::string& addr, unsigned int port) : addr_(addr), port_(port) {}
  TiedServer(const TiedServer& other) : servers_(other.servers_), addr_(other.addr_), port_(other.port_) {}
  ~TiedServer() {}
  TiedServer& operator=(const TiedServer& other) {
    if (this != &other) {
      this->servers_ = other.servers_;
      this->addr_ = other.addr_;
      this->port_ = other.port_;
    }
    return *this;
  }

  std::vector<const config::Server*> servers_;
  std::string addr_;
  unsigned int port_;
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler {
 public:
  NetworkIOHandler();
  NetworkIOHandler(const NetworkIOHandler&);
  ~NetworkIOHandler();
  NetworkIOHandler& operator=(const NetworkIOHandler&);
  int setupSocket(const std::string& address, unsigned int port);
  ssize_t receiveRequest(ConnectionManager& conn_manager, int sock);
  ssize_t sendResponse(ConnectionManager& conn_manager, int sock);
  ssize_t receiveCgiResponse(ConnectionManager& conn_manager, int sock);
  ssize_t sendRequestBody(ConnectionManager& conn_manager, int sock);
  int acceptConnection(ConnectionManager& conn_manager, int listen_fd);
  void closeConnection(ConnectionManager& conn_manager, IServer* server, TimerTree& timer_tree, int sock);
  void purgeConnection(ConnectionManager& conn_manager, IServer* server, TimerTree& timer_tree, int sock);
  void closeAllListenSockets();
  const std::map<int, TiedServer>& getListenfdMap();
  void addVServer(int listen_fd, const TiedServer& server);
  bool isListenSocket(int listen_fd) const;
  static size_t getBufferSize();

 private:
  std::map<int, TiedServer> listenfd_map_;  // リスニングソケットとそれに紐づくserver configを管理
  static const size_t buffer_size_ = 1024;
};

#endif
