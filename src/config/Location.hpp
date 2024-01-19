#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <vector>
#include <set>
#include <string>

#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Alias.hpp"
#include "Allow.hpp"
#include "Deny.hpp"
#include "Index.hpp"
#include "KeepAliveTimeout.hpp"
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
class Location
{
	public:
		Location(const std::string &uri) : uri_(uri) {}
		~Location() {}
		std::set<std::string>	set_directives;
		const std::string	uri_;
		std::vector<AccessLog>	access_log_list;
		std::vector<ErrorLog>	error_log_list;
		std::vector<ErrorPage>	error_page_list;
		Alias	alias; 
		std::vector<Allow>	allow_list;
		std::vector<Deny>	deny_list;
		std::vector<Index>	index_list;
		KeepaliveTimeout	keepalive_timeout;
		std::vector<Return>	return_list;
		Root	root;
		TryFiles	try_files;
		Userid	userid;
		UseridDomain	userid_domain;
		UseridExpires	userid_expires;
		UseridPath	userid_path;
		UseridService	user_service;
};
} // namespace config

#endif
