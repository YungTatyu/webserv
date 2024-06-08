#include "SysCallWrapper.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "WebServer.hpp"
#include "error.hpp"

int SysCallWrapper::Socket(int domain, int type, int protocol) {
  int listenfd = socket(domain, type, protocol);
  if (listenfd == -1) throw std::runtime_error(error::strSysCallError("socket") + "\n");
  return listenfd;
}

int SysCallWrapper::Setsockopt(int socket, int level, int option_name, const void *option_value,
                               socklen_t option_len) {
  int re = setsockopt(socket, level, option_name, option_value, option_len);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("setsockopt") + "\n");
  return re;
}

int SysCallWrapper::Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
  return bind(socket, address, address_len);
}

int SysCallWrapper::Listen(int socket, int backlog) {
  int re = listen(socket, backlog);
  if (re == -1) throw std::runtime_error(error::strSysCallError("listen") + "\n");
  return re;
}

int SysCallWrapper::Accept(int socket, struct sockaddr *address, socklen_t *address_len) {
  int connfd = accept(socket, address, address_len);
  if (connfd == -1) WebServer::writeErrorlog(error::strSysCallError("accept") + "\n");
  return connfd;
}

pid_t SysCallWrapper::Fork(void) {
  int re = fork();
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("fork") + "\n");
  return re;
}

int SysCallWrapper::Dup2(int fildes, int fildes2) {
  int re = dup2(fildes, fildes2);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("dup2") + "\n");
  return re;
}

int SysCallWrapper::Fcntl(int fd, int cmd, int flags) {
  // TODO: close-on-execをセットするなら、F_SETFDを使わないといけなさそう（subjectで使用不可のフラグ）
  int re = fcntl(fd, cmd, flags);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("fctnl") + "\n");
  return re;
}

int SysCallWrapper::Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int re = getsockname(sockfd, addr, addrlen);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("getsockname") + "\n");
  return re;
}
