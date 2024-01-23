#include "SysCallWrapper.hpp"

int SysCallWrapper::Socket( int domain, int type, int protocol )
{
	int listenfd = socket( domain, type, protocol );
	if ( listenfd == -1 )
	{
		perror( "socket" );
		throw std::runtime_error( "socket" );
	}
	return listenfd;
}

int SysCallWrapper::Setsockopt( int socket, int level, int option_name, const void *option_value, socklen_t option_len )
{
	int re;
	re = setsockopt( socket, level, option_name, option_value, option_len );
	if ( re == -1 )
	{
		perror( "setsockopt" );
		throw std::runtime_error( "setsockopt" );
	}
	return re;	
}

int SysCallWrapper::Bind( int socket, const struct sockaddr *address, socklen_t address_len )
{
	int re;
	re =  bind( socket, address, address_len );
	if ( re == -1 )
	{
		perror( "bind" );
		throw std::runtime_error( "bind" );
	}
	return re;
}

int SysCallWrapper::Listen(int socket, int backlog)
{
	int re = listen( socket, backlog );
	if ( re == -1 )
	{
		perror( "listen" );
		throw std::runtime_error( "listen" );
	}
	return re;
}

// int SysCallWrapper::Accept( int socket, struct sockaddr *restrict address, socklen_t *restrict address_len )
int SysCallWrapper::Accept( int socket, struct sockaddr *address, socklen_t *address_len )
{
	int connfd = accept( socket, address, address_len );
	if ( connfd == -1 )
	{
		perror( "accept" );
	}
	return connfd;
}

int SysCallWrapper::Poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
	return ( poll( fds, nfds, timeout ) );
}

