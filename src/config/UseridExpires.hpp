#ifndef CONFIG_USERID_EXPIRES_HPP
#define CONFIG_USERID_EXPIRES_HPP

#include "Time.hpp"
#include "conf.hpp"

namespace config {
class UseridExpires {
 private:
  Time time_;
  bool is_userid_expires_on_;

 public:
  UseridExpires() : is_userid_expires_on_(this->kDefaultIsUseridExpiresOn) {}
  UseridExpires(const UseridExpires& other) { *this = other; }
  ~UseridExpires() {}
  UseridExpires& operator=(const UseridExpires& other) {
    if (this != &other) {
      this->time_ = other.time_;
      this->is_userid_expires_on_ = other.is_userid_expires_on_;
    }
    return *this;
  }
  const Time& getTime() const { return this->time_; }
  bool getIsUseridExpiresOn() const { return this->is_userid_expires_on_; }
  void setTime(unsigned long time_in_ms) { this->time_.time_in_ms_ = time_in_ms; }
  void setIsUseridExpiresOn(bool is_userid_expires_on) { this->is_userid_expires_on_ = is_userid_expires_on; }
  static const bool kDefaultIsUseridExpiresOn = false;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
