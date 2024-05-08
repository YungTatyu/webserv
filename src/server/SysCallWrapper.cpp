#include "SysCallWrapper.hpp"

#include <fcntl.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "error.hpp"

int SysCallWrapper::Socket(int domain, int type, int protocol) {
  int listenfd = socket(domain, type, protocol);
  if (listenfd == -1) {
    perror("socket");
    throw std::runtime_error("socket");
  }
  return listenfd;
}

int SysCallWrapper::Setsockopt(int socket, int level, int option_name, const void *option_value,
                               socklen_t option_len) {
  int re = setsockopt(socket, level, option_name, option_value, option_len);
  if (re == -1)
    std::cerr << error::strerror("setsockopt") << std::endl;
  return re;
}

int SysCallWrapper::Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
  int re;
  re = bind(socket, address, address_len);
  if (re == -1) {
    perror("bind");
    throw std::runtime_error("bind");
  }
  return re;
}

int SysCallWrapper::Listen(int socket, int backlog) {
  int re = listen(socket, backlog);
  if (re == -1) {
    perror("listen");
    throw std::runtime_error("listen");
  }
  return re;
}

// int SysCallWrapper::Accept( int socket, struct sockaddr *restrict address, socklen_t *restrict address_len
// )
int SysCallWrapper::Accept(int socket, struct sockaddr *address, socklen_t *address_len) {
  int connfd = accept(socket, address, address_len);
  if (connfd == -1) {
    perror("accept");
  }
  return connfd;
}

int SysCallWrapper::Pipe(int fildes[2]) {
  int re = pipe(fildes);
  if (re == -1) {
    perror("pipe");
  }
  return re;
}

pid_t SysCallWrapper::Fork(void) {
  int re = fork();
  if (re == -1) {
    perror("fork");
  }
  return re;
}

int SysCallWrapper::Dup2(int fildes, int fildes2) {
  int re = dup2(fildes, fildes2);
  if (re == -1) {
    perror("dup2");
    std::exit(EXIT_FAILURE);
  }
  return re;
}

int SysCallWrapper::Fcntl(int fd, int cmd, int flags) {
  // TODO: close-on-execをセットするなら、F_SETFDを使わないといけなさそう（subjectで使用不可のフラグ）
  int re = fcntl(fd, cmd, flags);
  if (re == -1) std::cerr << "webserv: [emerg] fcntl (" << errno << ":" << std::strerror(errno) << ")\n";
  return re;
}
