#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <vector>
#include <set>
#include <string>

#include "conf.hpp"
#include "LimitExcept.hpp"
#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Alias.hpp"
#include "Allow.hpp"
#include "Deny.hpp"
#include "Index.hpp"
#include "Autoindex.hpp"
#include "KeepaliveTimeout.hpp"
#include "Return.hpp"
#include "Root.hpp"
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
	Location(const std::string &uri) : uri_(uri) {}
	~Location() {}
	const static unsigned int	type = CONF_HTTP_SERVER|CONF_TAKE1|CONF_NOT_UNIQUE;
	std::set<std::string>	set_directives;
	const std::string	uri_;
	LimitExcept	limit_except;
	std::vector<AccessLog>	access_log_list;
	std::vector<ErrorLog>	error_log_list;
	std::vector<ErrorPage>	error_page_list;
	Alias	alias; 
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
	std::vector<Index>	index_list;
	Autoindex	autoindex;
	KeepaliveTimeout	keepalive_timeout;
	std::vector<Return>	return_list;
	Root	root;
	TryFiles	try_files;
	Userid	userid;
	UseridDomain	userid_domain;
	UseridExpires	userid_expires;
	UseridPath	userid_path;
	UseridService	userid_service;
};
} // namespace config

#endif
