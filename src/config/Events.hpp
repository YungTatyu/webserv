#ifndef CONFIG_EVENTS_HPP
#define CONFIG_EVENTS_HPP

#include <set>
#include <string>

#include "Use.hpp"
#include "WorkerConnections.hpp"
#include "conf.hpp"

namespace config {
struct Events {
  const static unsigned int kType_ = CONF_MAIN | CONF_NOARGS | CONF_UNIQUE;
  std::set<std::string> directives_set_;
  WorkerConnections worker_connections_;
  Use use_;
};
}  // namespace config

#endif