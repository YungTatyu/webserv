#ifndef CONFIG_RETURN_HPP
#define CONFIG_RETURN_HPP

#include <algorithm>
#include <string>

#include "conf.hpp"

namespace config {
class Return {
 private:
  int code_;
  std::string url_;  // url or text
  static const int kRedirectCodesSize = 5;

 public:
  Return() : code_(this->kCodeUnset_) {}
  Return(const Return &other) { *this = other; }
  ~Return() {}
  Return &operator=(const Return &other) {
    if (this != &other) {
      this->code_ = other.code_;
      this->url_ = other.url_;
    }
    return *this;
  }
  const int &getCode() const { return this->code_; }
  const std::string &getUrl() const { return this->url_; }
  void setCode(int code) { this->code_ = code; }
  void setUrl(const std::string &url) { this->url_ = url; }
  static bool isRedirectCode(int code) {
    for (size_t i = 0; i < kRedirectCodesSize; ++i) {
      if (kRedirectCodes_[i] == code) return true;
    }
    return false;
  };
  static const int kCodeUnset_ = -1;
  static const int kRedirectCodes_[kRedirectCodesSize];
  static const unsigned int kType_ = CONF_HTTP_LOCATION | CONF_TAKE12 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
