#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include <vector>
#include <set>
#include <string>

#include "conf.hpp"
#include "Server.hpp"
#include "AccessLog.hpp"
#include "SendTimeout.hpp"
#include "KeepaliveTimeout.hpp"
#include "ErrorPage.hpp"
#include "ErrorLog.hpp"
#include "Root.hpp"
#include "ClientMaxBodySize.hpp"
#include "AllowDeny.hpp"
#include "Index.hpp"
#include "Autoindex.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"
#include "KeepaliveTimeout.hpp"

namespace config
{
struct Http
{
	const static unsigned int	type = CONF_MAIN|CONF_NOARGS|CONF_UNIQUE;
	std::set<std::string>	directives_set;
	std::vector<Server>	server_list;
	Root	root;
	std::vector<Index>	index_list;
	Autoindex	autoindex;
	ClientMaxBodySize	client_max_body_size;
	std::vector<AccessLog>	access_log_list;
	std::vector<ErrorLog>	error_log_list;
	std::vector<ErrorPage>	error_page_list;
	KeepaliveTimeout	keepalive_timeout;
	SendTimeout	send_timeout;
	std::vector<AllowDeny>	allow_deny_list;
	Userid	userid;
	UseridDomain	userid_domain;
	UseridExpires	userid_expires;
	UseridPath	userid_path;
	UseridService	userid_service;
	std::vector<int>	access_fd_list;
	std::vector<int>	error_fd_list;
};
} // namespace config


#endif
