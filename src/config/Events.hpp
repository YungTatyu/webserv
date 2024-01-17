#ifndef CONFIG_EVENTS_HPP
#define CONFIG_EVENTS_HPP

#include "WorkerConnections.hpp"
#include "Use.hpp"

namespace config
{
struct Events
{
	WorkerConnections	worker_connections;
	Use	use;
};
} // namespace config

#endif