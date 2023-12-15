#ifndef CONF_HPP
#define CONF_HPP

#include <iostream>
#include <vector>

/*
 * context level:
 * main -> http -> server -> location
 * 
**/

struct CErrorLog
{
	enum errorlog_level
	{
		DEBUG,
		INFO,
		NOTICE,
		WARN,
		ERROR,
		CRIT,
		ALERT,
		EMERG
	};
	errorlog_level	level;
};

// level: server
struct CServerListen
{
	std::string	address; // default:0.0.0.0
	unsigned int	port; // default:80
};

// serverの要素
struct CServerLocation
{
	std::string	root;
	std::string	uri;
	
};

// level: http, server, location
struct CErrorPage
{
	std::vector<unsigned int>	statuses;
	int	response; //optional
	std::string uri;
};

// level: http, server, location
struct CPermission
{
	std::vector<std::string> allow;
	std::vector<std::string> deny;
};

// level: location
struct CLocationLimitExpect
{
	std::string	method;
	CPermission	permission;
};

// level: server
struct CServerLocation
{
	std::string	root;
	std::string	uri;
	unsigned int	send_timeout; // default:60s
	unsigned int	keepalive_timeout; // default:75s
	unsigned int	client_max_body_size; // Default:1m, sizeを0にすると制限なし 
	bool	autoindex;
	std::string	index;
};

// 一つのserverの設定、serverの要素を全て持つ
struct CServer
{
	std::string	root;
	std::vector<std::string>	server_names;
	CServerListen	listen;
	std::vector<CServerLocation>	locations;
	bool	autoindex;
	std::string	index;
};

// level: main
struct CHTTP
{
	std::string	root;
	std::vector<CServer>	servers;
	bool	autoindex;
	std::string	index;
};

class Conf
{
	private: // level: main
		CErrorLog	errorlog;
		CHTTP	http;
	public:
		Conf();
		~Conf();
};

#endif