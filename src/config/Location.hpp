#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <vector>

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

namespace config
{
struct Location
{
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
};
} // namespace config

#endif
