#include "NetworkIOHandler.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

#include "ConnectionManager.hpp"
#include "IServer.hpp"
#include "TimerTree.hpp"
#include "utils.hpp"
#include "WebServer.hpp"
#include "conf.hpp"
#include "error.hpp"
#include "syscall_wrapper.hpp"

const size_t NetworkIOHandler::buffer_size_;

NetworkIOHandler::NetworkIOHandler() {}

NetworkIOHandler::~NetworkIOHandler() {
  closeAllListenSockets();  // リスニングソケットのクローズ
}

/* NetworkIOHandlerクラスの実装 */
int NetworkIOHandler::setupSocket(const std::string& address, unsigned int port) {
  try {
    // creation of the socket
#if defined(SOCK_NONBLOCK) && defined(SOCK_CLOEXEC)
    int listen_fd = syscall_wrapper::Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
#else
    int listen_fd = syscall_wrapper::Socket(AF_INET, SOCK_STREAM, 0);
    utils::setNonBlockingCloExec(listen_fd);
#endif

    // socketがtimeout中でもbindできるよう開発中はして、すぐにサーバを再起動できるようにする。
    int yes = 1;
    int re = syscall_wrapper::Setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (re != 0) std::exit(EXIT_FAILURE);

    // preparation of the socket address
    struct sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(utils::strToIPAddress(address));
    servaddr.sin_port = htons(port);

    // 失敗したとき？
    re = syscall_wrapper::Bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (re == -1)
      throw std::runtime_error(error::strSysCallError("bind", "to " + address + ":" + utils::toStr(port)));

    syscall_wrapper::Listen(listen_fd, SOMAXCONN);

    std::cout << "Server running on port " << port << std::endl;
    return listen_fd;
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

void NetworkIOHandler::addVServer(int listen_fd, const TiedServer& server) {
  this->listenfd_map_.insert(std::make_pair(listen_fd, server));
}

ssize_t NetworkIOHandler::receiveRequest(ConnectionManager& connManager, int sock) {
  std::vector<unsigned char> buffer(buffer_size_);
  int flag = 0;
#if defined(MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
#endif
  ssize_t re = recv(sock, buffer.data(), buffer_size_, flag);
  if (re > 0) connManager.addRawRequest(sock, buffer, re);
  return re;
}

ssize_t NetworkIOHandler::receiveCgiResponse(ConnectionManager& connManager, int sock) {
  std::vector<unsigned char> buffer(buffer_size_);
  int flag = 0;
#if defined(MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
#endif
  ssize_t re = recv(sock, buffer.data(), buffer_size_, flag);
  if (re > 0) connManager.addCgiResponse(sock, buffer, re);
  return re;
}

ssize_t NetworkIOHandler::sendResponse(ConnectionManager& connManager, int sock) {
  std::vector<unsigned char> response = connManager.getFinalResponse(sock);
  size_t res_size = response.size();
  size_t sent_bytes = connManager.getConnection(sock)->sent_bytes_;
  size_t cur_chunk_size = std::min(buffer_size_, res_size - sent_bytes);
  int flag = 0;
#if defined(MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
#endif
  ssize_t sent = send(sock, response.data() + sent_bytes, cur_chunk_size, flag);
  if (sent > 0) connManager.addSentBytes(sock, sent);
  return sent;
}

ssize_t NetworkIOHandler::sendRequestBody(ConnectionManager& connManager, int sock) {
  const std::string& body = connManager.getRequest(sock).body_;
  const size_t sent_bytes = connManager.getSentBytes(sock);
  const size_t rest = body.size() - sent_bytes;
  size_t cur_chunk_size = std::min(buffer_size_, rest);
  if (cur_chunk_size == 0) return 1;  // すでにresponseを全て送信しきっていたら、send終了
  int flag = 0;
#if defined(MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
#endif
  ssize_t re = send(sock, body.data() + sent_bytes, std::min(buffer_size_, rest), flag);
  if (re > 0) connManager.addSentBytes(sock, re);
  return re;
}

int NetworkIOHandler::acceptConnection(ConnectionManager& connManager, int listen_fd) {
  int connfd;
  struct sockaddr_in cliaddr;
  socklen_t client;

  client = sizeof(cliaddr);
  connfd = syscall_wrapper::Accept(listen_fd, (struct sockaddr*)&cliaddr, &client);
  if (connfd == -1) return connfd;
  utils::setNonBlockingCloExec(connfd);
#if defined(SO_NOSIGPIPE)
  int opt = 1;
  syscall_wrapper::Setsockopt(connfd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

  // 新規クライントfdを追加
  connManager.setConnection(connfd);
  connManager.setTiedServer(connfd, &this->listenfd_map_[listen_fd]);

  // show ip address of newly connected client.
  char clientIp[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
  std::cout << "> New client connected from IP: " << clientIp << std::endl;
  return connfd;
}

bool NetworkIOHandler::isListenSocket(int listen_fd) const {
  return this->listenfd_map_.find(listen_fd) != this->listenfd_map_.end();
}

/**
 * @brief connectionとそれに紐づくtimerを消す
 *
 * @param connManager
 * @param sock
 */
void NetworkIOHandler::closeConnection(ConnectionManager& connManager, IServer* server, TimerTree& timerTree,
                                       int sock) {
  server->deleteEvent(sock, connManager.getEvent(sock));
  close(sock);
  timerTree.deleteTimer(sock);
  bool cgi = connManager.isCgiSocket(sock);
  if (cgi) connManager.resetCgiSockets(sock);
  connManager.removeConnection(sock, cgi);
}

/**
 * @brief clientとのコネクションを完全に削除する
 * client, cgi socket, timerを全て削除: clientとcgiの両方を削除しなければいけない時に、この関数を呼ぶ
 *
 */
void NetworkIOHandler::purgeConnection(ConnectionManager& connManager, IServer* server, TimerTree& timerTree,
                                       int sock) {
  server->deleteEvent(sock, connManager.getEvent(sock));
  close(sock);
  timerTree.deleteTimer(sock);
  bool cgi = connManager.isCgiSocket(sock);
  if (!cgi) {
    connManager.removeConnection(sock, cgi);
    return;
  }
  // cgi socketの場合は、clientのデータも削除する
  const cgi::CgiHandler& cgi_handler = connManager.getCgiHandler(sock);
  int client = cgi_handler.getCliSocket();
  close(client);
  // cgiのコネクションを最初に削除する
  connManager.removeConnection(sock, cgi);
  connManager.removeConnection(client, false);
}

const std::map<int, TiedServer>& NetworkIOHandler::getListenfdMap() { return this->listenfd_map_; }

size_t NetworkIOHandler::getBufferSize() { return buffer_size_; }

void NetworkIOHandler::closeAllListenSockets() {
  for (std::map<int, TiedServer>::iterator it = this->listenfd_map_.begin(); it != this->listenfd_map_.end();
       ++it) {
    close(it->first);
  }
  this->listenfd_map_.clear();
}
