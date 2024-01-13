#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include <vector>

#include "AccessLog.hpp"
#include "SendTimeout.hpp"
#include "KeepaliveTime.hpp"
#include "KeepaliveTimeout.hpp"
#include "ErrorPage.hpp"
#include "Root.hpp"
#include "ClientMaxBodySize.hpp"
#include "Allow.hpp"
#include "Deny.hpp"
#include "Index.hpp"
#include "Autoindex.hpp"
#include "Allow.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"

namespace config
{
struct Http
{
	Root	root;
	Index	index;
	Autoindex	autoindex;
	ClientMaxBodySize	client_max_body_size;
	std::vector<AccessLog>	access_log_list;
	SendTimeout	send_timeout;
	KeepaliveTime	keepalive_time;
	KeepaliveTimeout	keepalive_timeout;
	ErrorPage	error_page;
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
	Userid	userid;
	UseridDomain	userid_domain;
	UseridExpires	userid_expires;
	UseridPath	userid_path;
};
} // namespace config


#endif