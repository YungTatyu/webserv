#ifndef CONFIG_RECEIVE_TIMEOUT_HPP
#define CONFIG_RECEIVE_TIMEOUT_HPP

#include "Time.hpp"
#include "conf.hpp"

namespace config {
class ReceiveTimeout {
 private:
  Time time_;

 public:
  ReceiveTimeout() : time_(this->kDefaultTime_) {}
  ReceiveTimeout(const ReceiveTimeout& other) { *this = other; }
  ~ReceiveTimeout() {}
  ReceiveTimeout& operator=(const ReceiveTimeout& other) {
    if (this != &other) {
      this->time_ = other.time_;
    }
    return *this;
  }
  const Time& getTime() const { return this->time_; }
  void setTime(unsigned long time_in_ms) { this->time_.time_in_ms_ = time_in_ms; }
  static const unsigned long kDefaultTime_;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
