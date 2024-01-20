#ifndef SYS_CALL_WRAPPER_HPP
# define SYS_CALL_WRAPPER_HPP

# include <stdexcept>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <stdio.h>

namespace SysCallWrapper
{
	int Socket( int domain, int type, int protocol );
	int Setsockopt( int socket, int level, int option_name, const void *option_value, socklen_t option_len );
	int Bind( int socket, const struct sockaddr *address, socklen_t address_len );
	int Listen(int socket, int backlog);
	// int Accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len); C++ではrestrictキーワードは使えないのでラッパーでは指定しない。
	int Accept( int socket, struct sockaddr *address, socklen_t *address_len );
}

#endif

