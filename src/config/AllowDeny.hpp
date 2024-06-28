#ifndef CONFIG_ALLOW_DENY_HPP
#define CONFIG_ALLOW_DENY_HPP

#include <string>

#include "conf.hpp"

namespace config {
enum ACCESS_DIRECTIVE {
  ALLOW,
  DENY
};

class AllowDeny {
 private:
  std::string address_;
  ACCESS_DIRECTIVE access_directive_;

 public:
  AllowDeny() {}
  AllowDeny(const AllowDeny& other) { *this = other; }
  ~AllowDeny() {}
  AllowDeny& operator=(const AllowDeny& other) {
    if (this != &other) {
      this->address_ = other.address_;
      this->access_directive_ = other.access_directive_;
    }
    return *this;
  }
  const std::string& getAddress() const { return this->address_; };
  void setAddress(const std::string& address) { this->address_ = address; };
  ACCESS_DIRECTIVE getAccessDirective() const { return this->access_directive_; };
  void setAccessDirective(ACCESS_DIRECTIVE access_directive) { this->access_directive_ = access_directive; };
  static const unsigned int kType_ = CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION |
                                     CONF_HTTP_LIMIT_EXCEPT | CONF_TAKE1 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
