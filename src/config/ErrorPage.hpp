#ifndef CONFIG_ERROR_PAGE_HPP
#define CONFIG_ERROR_PAGE_HPP

#include <string>
#include <vector>

#include "conf.hpp"

namespace config {
class ErrorPage {
 private:
  std::set<unsigned int> code_set_;
  long response_;
  std::string uri_;

 public:
  ErrorPage() : response_(this->kResponseUnset) {}
  ErrorPage(const ErrorPage &other) { *this = other; }
  ~ErrorPage() {}
  ErrorPage &operator=(const ErrorPage &other) {
    if (this != &other) {
      this->code_set_ = other.code_set_;
      this->response_ = other.response_;
      this->uri_ = other.uri_;
    }
    return *this;
  }
  const std::set<unsigned int> &getCodeList() const { return this->code_set_; }
  const long &getResponse() const { return this->response_; }
  const std::string &getUri() const { return this->uri_; }
  void addCode(unsigned int code) { this->code_set_.insert(code); }
  void setResponse(long response) { this->response_ = response; }
  void setUri(const std::string &uri) { this->uri_ = uri; }
  const static long kResponseUnset = -1;
  const static unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_1MORE | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
