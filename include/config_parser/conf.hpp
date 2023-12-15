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
	errorlog_level	level_;
};

// level: http, server, location
struct CErrorPage
{
	std::vector<unsigned int>	statuses_;
	std::string	uri_;
};


// level: server
struct CServerListen
{
	std::string	address_; // default:0.0.0.0
	unsigned int	port_; // default:80
};

// serverの要素
struct CServerLocation
{
	std::string	root_;
	std::string	uri_;
	
};

// level: http, server, location
struct CErrorPage
{
	std::vector<unsigned int>	statuses_;
	int	response_; //optional
	std::string uri_;
};

// level: http, server, location
struct CPermission
{
	std::vector<std::string> allow_;
	std::vector<std::string> deny_;
};

// level: location
struct CLocationLimitExpect
{
	std::string	method_;
	CPermission	permission_;
};

// level: server
struct CServerLocation
{
	std::string	root_;
	std::string	uri_;
	unsigned int	send_timeout_; // default:60s
	unsigned int	keepalive_timeout_; // default:75s
	unsigned int	client_max_body_size_; // Default:1m, sizeを0にすると制限なし 
	bool	autoindex_;
	std::string	index_;
	CErrorPage	error_page_;
};

// serverの設定
struct CServer
{
	std::string	root_;
	std::vector<std::string>	server_names_;
	CServerListen	listen_;
	std::vector<CServerLocation>	locations_;
	bool	autoindex_;
	CErrorPage	error_page_;
	std::string	index_;
};

// httpの設定
struct CHTTP
{
	std::string	root_;
	std::vector<CServer>	servers_;
	CErrorPage	error_page_;
	bool	autoindex_;
	std::string	index_;
};

class Conf
{
	private: // level: main
		CErrorLog	errorlog_;
		CHTTP	http_;
	public:
		Conf();
		~Conf();
};

#endif