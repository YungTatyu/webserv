#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <vector>

#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "Alias.hpp"
#include "Allow.hpp"
#include "Deny.hpp"

namespace config
{
struct Location
{
	std::vector<AccessLog>	access_log_list;
	std::vector<ErrorLog>	error_log_list;
	Alias	alias;
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;

};
} // namespace config

#endif
