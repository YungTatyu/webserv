#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <vector>
#include <set>
#include <string>

#include "conf.hpp"
#include "LimitExcept.hpp"
#include "AccessLog.hpp"
#include "Alias.hpp"
#include "AllowDeny.hpp"
#include "Autoindex.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Index.hpp"
#include "KeepaliveTimeout.hpp"
#include "Return.hpp"
#include "Root.hpp"
#include "SendTimeout.hpp"
#include "TryFiles.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"

namespace config
{
struct Location
{
	Location(const std::string &_uri) : uri(_uri) {}
	~Location() {}
	const static unsigned int	type = CONF_HTTP_SERVER|CONF_TAKE1|CONF_NOT_UNIQUE;
	std::set<std::string>	directives_set;
	const std::string	uri;
	LimitExcept	limit_except;
	std::vector<AccessLog>	access_log_list;
	std::vector<ErrorLog>	error_log_list;
	std::vector<ErrorPage>	error_page_list;
	Alias	alias; 
	std::vector<AllowDeny>	allow_deny_list;
	std::vector<Index>	index_list;
	Autoindex	autoindex;
	KeepaliveTimeout	keepalive_timeout;
	SendTimeout	send_timeout;
	std::vector<Return>	return_list;
	Root	root;
	TryFiles	try_files;
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
