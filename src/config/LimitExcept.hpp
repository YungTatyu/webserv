#ifndef CONFIG_LIMITEXCEPT_HPP
#define CONFIG_LIMITEXCEPT_HPP

#include <set>
#include <string>
#include <vector>

#include "AllowDeny.hpp"
#include "Use.hpp"
#include "conf.hpp"

namespace config {
enum REQUEST_METHOD {
  UNKNOWN,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE
};

struct LimitExcept {
  LimitExcept() {}
  LimitExcept(const LimitExcept& other) { *this = other; }
  ~LimitExcept() {}
  LimitExcept& operator=(const LimitExcept& other) {
    if (this != &other) {
      this->directives_set_ = other.directives_set_;
      this->excepted_methods_ = other.excepted_methods_;
      this->allow_deny_list_ = other.allow_deny_list_;
    }
    return *this;
  }
  static std::string MethodToStr(const REQUEST_METHOD method) {
    std::string re;
    switch (method) {
      case GET:
        re = "GET";
        break;
      case HEAD:
        re = "HEAD";
        break;
      case POST:
        re = "POST";
        break;
      case PUT:
        re = "PUT";
        break;
      case DELETE:
        re = "DELETE";
        break;
      case UNKNOWN:
        break;
    }
    return re;
  }
  std::set<std::string> directives_set_;
  std::set<REQUEST_METHOD> excepted_methods_;  // limit_exceptの制限から除外されるmethod
  std::vector<AllowDeny> allow_deny_list_;
  static const unsigned int kType_ = CONF_HTTP_LOCATION | CONF_1MORE | CONF_UNIQUE;
};
}  // namespace config

#endif
