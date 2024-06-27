#ifndef CONFIG_WORKER_CONNECTIONS_HPP
#define CONFIG_WORKER_CONNECTIONS_HPP

#include "conf.hpp"
#include "sys/socket.h"

namespace config {
class WorkerConnections {
 private:
  unsigned long worker_connections_;

 public:
  WorkerConnections() : worker_connections_(this->kDefaultWorkerConnections_) {}
  WorkerConnections(const WorkerConnections& other) { *this = other; }
  ~WorkerConnections() {}
  WorkerConnections& operator=(const WorkerConnections& other) {
    if (this != &other) {
      this->worker_connections_ = other.worker_connections_;
    }
    return *this;
  }
  const unsigned long& getWorkerConnections() const { return this->worker_connections_; }
  void setWorkerConnections(unsigned long worker_connections) {
    this->worker_connections_ = worker_connections;
  }
  const static unsigned long kSelectMaxConnections = FD_SETSIZE;
  const static unsigned long kDefaultWorkerConnections_ = 512;
  const static unsigned int kType_ = CONF_EVENTS | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
