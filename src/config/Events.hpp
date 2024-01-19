#ifndef CONFIG_EVENTS_HPP
#define CONFIG_EVENTS_HPP

#include <set>
#include <string>

#include "WorkerConnections.hpp"
#include "Use.hpp"

namespace config
{
struct Events
{
	std::set<std::string>	set_directives;
	WorkerConnections	worker_connections;
	Use	use;
};
} // namespace config

#endif