#ifndef CONFIG_KEEPALIVE_TIMEOUT_HPP
#define CONFIG_KEEPALIVE_TIMEOUT_HPP

#include "Time.hpp"
#include "conf.hpp"

namespace config {
class KeepaliveTimeout {
 private:
  Time time_;

 public:
  KeepaliveTimeout() : time_(this->kDefaultTime_) {}
  KeepaliveTimeout(const KeepaliveTimeout &other) { *this = other; }
  ~KeepaliveTimeout() {}
  KeepaliveTimeout &operator=(const KeepaliveTimeout &other) {
    if (this != &other) {
      this->time_ = other.time_;
    }
    return *this;
  }
  const Time &getTime() const { return this->time_; }
  void setTime(unsigned long time_in_ms) { this->time_.time_in_ms_ = time_in_ms; }
  static const unsigned long kDefaultTime_;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
