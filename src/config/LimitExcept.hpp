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
  DELETE,
};

struct LimitExcept {
  LimitExcept() {}
  ~LimitExcept() {}
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
  const static unsigned int type = CONF_HTTP_LOCATION | CONF_1MORE | CONF_UNIQUE;
  std::set<std::string> directives_set;
  std::set<REQUEST_METHOD> excepted_methods;  // limit_exceptの制限から除外されるmethod
  std::vector<AllowDeny> allow_deny_list;
};
}  // namespace config

#endif
