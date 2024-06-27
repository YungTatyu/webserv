#ifndef ACCESS_LOG_HPP
#define ACCESS_LOG_HPP

#include <string>

#include "conf.hpp"

namespace config {
class AccessLog {
 private:
  std::string file_;
  bool is_accesslog_on_;

 public:
  AccessLog() : file_(this->kDefaultFile_), is_accesslog_on_(this->kDefaultIsAccesslogOn_) {}
  AccessLog(const AccessLog &other) { *this = other; }
  ~AccessLog() {}
  AccessLog &operator=(const AccessLog &other) {
    if (this != &other) {
      this->file_ = other.file_;
      this->is_accesslog_on_ = other.is_accesslog_on_;
    }
    return *this;
  }
  const std::string &getFile() const { return this->file_; }
  const bool &getIsAccesslogOn() const { return this->is_accesslog_on_; }
  void setFile(const std::string &file) { this->file_ = file; }
  void setIsAccesslogOn(bool is_accesslog_on) { this->is_accesslog_on_ = is_accesslog_on; }
  const static char *kDefaultFile_;
  const static bool kDefaultIsAccesslogOn_ = true;
  const static unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
