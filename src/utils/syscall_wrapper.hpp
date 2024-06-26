#ifndef SYS_CALL_WRAPPER_HPP
#define SYS_CALL_WRAPPER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>

namespace syscall_wrapper {
int Open(const std::string& path, int flags, mode_t modes);
int Access(const std::string& path, int modes, bool err_log);
int Socket(int domain, int type, int protocol);
int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
// C++ではrestrictキーワードは使えないのでラッパーでは指定しない。
int Accept(int socket, struct sockaddr *address, socklen_t *address_len);
int Pipe(int fildes[2]);
pid_t Fork(void);
int Dup2(int fildes, int fildes2);
int Fcntl(int fd, int cmd, int flags);
int Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
}  // namespace syscall_wrapper

#endif
