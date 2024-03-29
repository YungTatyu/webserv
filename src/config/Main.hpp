#ifndef CONFIG_MAIN_HPP
#define CONFIG_MAIN_HPP

#include <vector>
#include <set>
#include <string>

#include "Events.hpp"
#include "ErrorLog.hpp"
#include "Http.hpp"

namespace config
{
struct Main
{
	std::set<std::string>	directives_set;
	std::vector<ErrorLog>	error_log_list;
	Events	events;
	Http	http;
	std::vector<int>	error_fd_list;
};

} // namespace config 

#endif
