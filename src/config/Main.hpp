#ifndef CONFIG_MAIN_HPP
#define CONFIG_MAIN_HPP

#include <vector>
#include "Events.hpp"
#include "ErrorLog.hpp"
#include "Http.hpp"
namespace config
{
struct Main
{
	std::vector<ErrorLog>	error_log_list;
	Events	events;
	Http	http;
};

} // namespace config 

#endif