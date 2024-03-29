#ifndef CONFIG_EVENTS_HPP
#define CONFIG_EVENTS_HPP

#include <set>
#include <string>

#include "conf.hpp"
#include "WorkerConnections.hpp"
#include "Use.hpp"

namespace config
{
struct Events
{
	const static unsigned int	type = CONF_MAIN|CONF_NOARGS|CONF_UNIQUE;
	std::set<std::string>	directives_set;
	WorkerConnections	worker_connections;
	Use	use;
};
} // namespace config

#endif