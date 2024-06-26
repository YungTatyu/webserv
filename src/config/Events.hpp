#ifndef CONFIG_EVENTS_HPP
#define CONFIG_EVENTS_HPP

#include <set>
#include <string>

#include "Use.hpp"
#include "WorkerConnections.hpp"
#include "conf.hpp"

namespace config {
struct Events {
  Events() {}
  Events(const Events& other) { *this = other; }
  ~Events() {}
  Events& operator=(const Events& other) {
    if (this != &other) {
      this->directives_set_ = other.directives_set_;
      this->worker_connections_ = other.worker_connections_;
      this->use_ = other.use_;
    }
    return *this;
  }
  const static unsigned int kType_ = CONF_MAIN | CONF_NOARGS | CONF_UNIQUE;
  std::set<std::string> directives_set_;
  WorkerConnections worker_connections_;
  Use use_;
};
}  // namespace config

#endif
