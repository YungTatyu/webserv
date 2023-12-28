#ifndef CONFIG_MAIN_HPP
#define CONFIG_MAIN_HPP

#include <vector>
#include "Events.hpp"
#include "ErrorLog.hpp"

namespace config
{
struct Main
{
	Events	events;
	std::vector<ErrorLog>	error_logs;
};

} // namespace config 

#endif