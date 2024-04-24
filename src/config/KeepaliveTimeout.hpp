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
  ~KeepaliveTimeout() {}
  const static unsigned long kDefaultTime_;
  const Time &getTime() const { return this->time_; }
  void setTime(const unsigned long &time_in_ms) { this->time_.time_in_ms_ = time_in_ms; }
  const static unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE12 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
