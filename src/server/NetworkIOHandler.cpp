#include "NetworkIOHandler.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "ConnectionManager.hpp"
#include "SysCallWrapper.hpp"

NetworkIOHandler::NetworkIOHandler() {}

NetworkIOHandler::~NetworkIOHandler() {
  closeAllListenSockets();  // リスニングソケットのクローズ
}

/* NetworkIOHandlerクラスの実装 */
int NetworkIOHandler::setupSocket(const std::string address, const unsigned int port) {
  try {
    // creation of the socket
    const int listen_fd = SysCallWrapper::Socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listen_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

    // socketがtimeout中でもbindできるよう開発中はして、すぐにサーバを再起動できるようにする。
    int yes = 1;
    int re = SysCallWrapper::Setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (re != 0) std::exit(EXIT_FAILURE);

    // preparation of the socket address
    struct sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    // TODO: strtoipaddressを適応する
    (void)address;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // 失敗したとき？
    SysCallWrapper::Bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    SysCallWrapper::Listen(listen_fd, SOMAXCONN);

    std::cout << "Server running on port " << port << std::endl;
    return listen_fd;
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

void NetworkIOHandler::addVServer(const int listen_fd, const TiedServer server) {
  this->listenfd_map_.insert(std::make_pair(listen_fd, server));
}

int NetworkIOHandler::receiveRequest(ConnectionManager& connManager, const int cli_sock) {
  std::vector<unsigned char> buffer(bufferSize_);

  int flag = 0;
  #if defined (MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
  #endif
  ssize_t re = recv(cli_sock, buffer.data(), bufferSize_, flag);
  if (re == 0)  // クライアントとのコネクションが閉じた時。
    return 0;
  else if (re == -1)  // ソケットが使用不可、またはエラー。
    return -1;

  connManager.addRawRequest(cli_sock, buffer, re);

  if (re == bufferSize_)  // bufferSize_分だけ読んだ時。次のループで残りを読む。
                          // ちょうどrecvでbuffersize分読んだ時はどうなる？？（次readイベント発生し 可能性）
    return 2;

  return 1;
}

int NetworkIOHandler::receiveCgiResponse(ConnectionManager& connManager, const int sock) {
  const static size_t buffer_size = 1024;
  std::vector<unsigned char> buffer(buffer_size);

  int flag = 0;
  #if defined (MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
  #endif
  ssize_t re = recv(sock, buffer.data(), buffer_size, flag);
  if (re == 0)  // cgi process died
    return 0;
  if (re == -1)  // error
    return -1;
  connManager.addCgiResponse(sock, buffer, re);
  if (re == buffer_size)  // continue recv
    return -2;
  return 1;
}

int NetworkIOHandler::sendResponse(ConnectionManager& connManager, const int cli_sock) {
  std::vector<unsigned char> response = connManager.getFinalResponse(cli_sock);
  size_t resSize = response.size();
  const size_t chunkSize = 1024;

  size_t sentBytes = connManager.getConnection(cli_sock)->sent_bytes_;
  size_t currentChunkSize = std::min(chunkSize, resSize - sentBytes);
  int flag = 0;
  #if defined (MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
  #endif
  int sent = send(cli_sock, response.data() + sentBytes, currentChunkSize, flag);
  if (sent == -1) return -1;
  connManager.getConnection(cli_sock)->sent_bytes_ += sent;
  if (connManager.getConnection(cli_sock)->sent_bytes_ == resSize)
    return 1;
  else
    return -2;
  return 0;  // clientが切断
}

ssize_t NetworkIOHandler::sendRequestBody(ConnectionManager& connManager, const int sock) {
  const static size_t buffer_size = 1024;
  const std::string body = connManager.getRequest(sock).body;
  const size_t sent_bytes = connManager.getSentBytes(sock);
  const size_t rest = body.size() - sent_bytes;

  int flag = 0;
  #if defined (MSG_NOSIGNAL)
  flag |= MSG_NOSIGNAL;
  #endif
  ssize_t re = send(sock, &body.c_str()[sent_bytes], std::min(buffer_size, rest), flag);
  if (re > 0) connManager.addSentBytes(sock, re);
  return re;
}

int NetworkIOHandler::acceptConnection(ConnectionManager& connManager, const int listen_fd) {
  int connfd;
  struct sockaddr_in cliaddr;
  socklen_t client;

  client = sizeof(cliaddr);
  connfd = SysCallWrapper::Accept(listen_fd, (struct sockaddr*)&cliaddr, &client);
  if (connfd == -1) return connfd;
  fcntl(connfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
  #if defined (SO_NOSIGPIPE)
  int opt = 1;
  SysCallWrapper::Setsockopt(connfd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
  #endif

  // 新規クライントfdを追加
  connManager.setConnection(connfd);
  connManager.setEvent(connfd, ConnectionData::EV_READ);
  connManager.setTiedServer(connfd, &this->listenfd_map_[listen_fd]);

  // show ip address of newly connected client.
  char clientIp[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
  std::cout << "> New client connected from IP: " << clientIp << std::endl;
  return connfd;
}

bool NetworkIOHandler::isListenSocket(const int listen_fd) const {
  try {
    this->listenfd_map_.at(listen_fd);
    return true;
  } catch (const std::out_of_range& e) {
    return false;
  }
}

void NetworkIOHandler::closeConnection(ConnectionManager& connManager, const int cli_sock) {
  close(cli_sock);
  bool cgi = connManager.isCgiSocket(cli_sock);
  if (cgi) connManager.resetCgiSockets(cli_sock);
  connManager.removeConnection(cli_sock, cgi);
  std::cerr << "client disconnected\n";
}

const std::map<int, TiedServer>& NetworkIOHandler::getListenfdMap() { return this->listenfd_map_; }

void NetworkIOHandler::closeAllListenSockets() {
  for (std::map<int, TiedServer>::iterator it = this->listenfd_map_.begin(); it != this->listenfd_map_.end();
       ++it) {
    close(it->first);
  }
  this->listenfd_map_.clear();
}
