#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include "AccessLog.hpp"
#include <vector>
namespace config
{
struct Http
{
	std::vector<AccessLog>	access_logs;
};
} // namespace config


#endif