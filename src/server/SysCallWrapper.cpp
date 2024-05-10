#include "SysCallWrapper.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "error.hpp"

int SysCallWrapper::Socket(int domain, int type, int protocol) {
  int listenfd = socket(domain, type, protocol);
  if (listenfd == -1) throw std::runtime_error(error::strSysCallError("socket"));
  return listenfd;
}

int SysCallWrapper::Setsockopt(int socket, int level, int option_name, const void *option_value,
                               socklen_t option_len) {
  int re = setsockopt(socket, level, option_name, option_value, option_len);
  if (re == -1) std::cerr << error::strSysCallError("setsockopt") << std::endl;
  return re;
}

int SysCallWrapper::Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
  int re;
  re = bind(socket, address, address_len);
  if (re == -1) throw std::runtime_error(error::strSysCallError("bind"));
  return re;
}

int SysCallWrapper::Listen(int socket, int backlog) {
  int re = listen(socket, backlog);
  if (re == -1) throw std::runtime_error(error::strSysCallError("listen"));
  return re;
}

int SysCallWrapper::Accept(int socket, struct sockaddr *address, socklen_t *address_len) {
  int connfd = accept(socket, address, address_len);
  if (connfd == -1) std::cerr << error::strSysCallError("accept") << std::endl;
  return connfd;
}

pid_t SysCallWrapper::Fork(void) {
  int re = fork();
  if (re == -1) std::cerr << error::strSysCallError("fork") << std::endl;
  return re;
}

int SysCallWrapper::Dup2(int fildes, int fildes2) {
  int re = dup2(fildes, fildes2);
  if (re == -1) std::cerr << error::strSysCallError("dup2") << std::endl;
  return re;
}

int SysCallWrapper::Fcntl(int fd, int cmd, int flags) {
  // TODO: close-on-execをセットするなら、F_SETFDを使わないといけなさそう（subjectで使用不可のフラグ）
  int re = fcntl(fd, cmd, flags);
  if (re == -1) std::cerr << error::strSysCallError("fcntl") << std::endl;
  return re;
}
