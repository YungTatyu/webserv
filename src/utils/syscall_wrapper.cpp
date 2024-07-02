#include "syscall_wrapper.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "WebServer.hpp"
#include "error.hpp"

int syscall_wrapper::Open(const std::string &path, int flags, mode_t modes) {
  int fd = open(path.c_str(), flags, modes);
  if (fd == -1) WebServer::writeErrorlog(error::strSysCallError("open", path) + "\n", config::EMERG);
  return fd;
}

int syscall_wrapper::Access(const std::string &path, int modes, bool err_log) {
  int ret = access(path.c_str(), modes);
  if (ret == -1 && err_log) WebServer::writeErrorlog(error::strSysCallError("access", path) + "\n", config::WARN);
  return ret;
}

int syscall_wrapper::Socket(int domain, int type, int protocol) {
  int listenfd = socket(domain, type, protocol);
  if (listenfd == -1) WebServer::writeErrorlog(error::strSysCallError("socket") + "\n", config::EMERG);
  return listenfd;
}

int syscall_wrapper::Setsockopt(int socket, int level, int option_name, const void *option_value,
                                socklen_t option_len) {
  int re = setsockopt(socket, level, option_name, option_value, option_len);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("setsockopt") + "\n", config::WARN);
  return re;
}

int syscall_wrapper::Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
  int re = bind(socket, address, address_len);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("bind") + "\n", config::EMERG);
  return re;
}

int syscall_wrapper::Listen(int socket, int backlog) {
  int re = listen(socket, backlog);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("listen") + "\n", config::EMERG);
  return re;
}

int syscall_wrapper::Accept(int socket, struct sockaddr *address, socklen_t *address_len) {
  int connfd = accept(socket, address, address_len);
  if (connfd == -1) WebServer::writeErrorlog(error::strSysCallError("accept") + "\n", config::WARN);
  return connfd;
}

pid_t syscall_wrapper::Fork(void) {
  int re = fork();
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("fork") + "\n", config::CRIT);
  return re;
}

int syscall_wrapper::Dup2(int fildes, int fildes2) {
  int re = dup2(fildes, fildes2);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("dup2") + "\n", config::WARN);
  return re;
}

int syscall_wrapper::Fcntl(int fd, int cmd, int flags) {
  // TODO: close-on-execをセットするなら、F_SETFDを使わないといけなさそう（subjectで使用不可のフラグ）
  int re = fcntl(fd, cmd, flags);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("fctnl") + "\n", config::WARN);
  return re;
}

int syscall_wrapper::Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int re = getsockname(sockfd, addr, addrlen);
  if (re == -1) WebServer::writeErrorlog(error::strSysCallError("getsockname") + "\n", config::WARN);
  return re;
}
