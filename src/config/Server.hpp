#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <vector>
#include <set>
#include <string>

#include "conf.hpp"
#include "Location.hpp"
#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Allow.hpp"
#include "Deny.hpp"
#include "Index.hpp"
#include "KeepaliveTimeout.hpp"
#include "Listen.hpp"
#include "Root.hpp"
#include "ServerName.hpp"
#include "TryFiles.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"

namespace config
{
struct Server
{
	const static unsigned int	type = CONF_HTTP_LOCATION|CONF_NOARGS|CONF_NOT_UNIQUE;
	std::set<std::string>	set_directives;
	std::vector<Location>	location_list;
	std::vector<AccessLog>	access_log_list;
	std::vector<ErrorLog>	error_log_list;
	std::vector<ErrorPage>	error_page_list;
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
	std::vector<Index>	index_list;
	std::vector<Listen>	listen_list;
	KeepaliveTimeout	keepalive_timeout;
	Root	root;
	std::vector<ServerName>	server_name_list;
	TryFiles	try_files;
	Userid	userid;
	UseridDomain	userid_domain;
	UseridExpires	userid_expires;
	UseridPath	userid_path;
	UseridService	user_service;
};
} // namespace config


#endif
