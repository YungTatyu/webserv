#ifndef CONF_HPP
#define CONF_HPP

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>

class listening
{
	private:
		int	socketfd_;
		struct sockaddr	*sockaddr_;
		socklen_t	socklen_; /* size of sockaddr */
		size_t	addr_text_max_len_;
		std::string	addr_;
	public:
		listening(/* args */);
		~listening();
};

class Connection
{
	private:
		long	timeout;
		event	read;
		event	write;
		log	error_log;
		listening	listening;
	public:
	Connection();
	~Connection();
};

class LocationContext
{
private:
	/* data */
public:
	LocationContext();
	~LocationContext();
};


class ServerContext
{
private:
	/* data */
public:
	ServerContext();
	~ServerContext();
};


class MainContext
{
private:
	/* data */
public:
	MainContext();
	~MainContext();
};


class directive
{
private:
	enum context
	{
		MAIN,
		CONNECTION,
		HTTP,
		SERVER,
		LOCATION,
	};
	std::string name;
	std::vector<std::string> args;
	context	context;
};

class context
{
private:
	std::string name;
	std::vector<class directive> directives;
	std::vector<class context> children;
};

#endif