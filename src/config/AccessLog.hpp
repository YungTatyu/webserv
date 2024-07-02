#ifndef ACCESS_LOG_HPP
#define ACCESS_LOG_HPP

#include <string>

#include "conf.hpp"

namespace config {
class AccessLog {
 private:
  int fd_;
  std::string file_;
  bool is_accesslog_on_;

 public:
  AccessLog() :
    fd_(-1),
    file_(this->kDefaultFile_),
    is_accesslog_on_(this->kDefaultIsAccesslogOn_) {}
  AccessLog(const AccessLog &other) { *this = other; }
  ~AccessLog() {}
  AccessLog &operator=(const AccessLog &other) {
    if (this != &other) {
      this->fd_ = other.fd_;
      this->file_ = other.file_;
      this->is_accesslog_on_ = other.is_accesslog_on_;
    }
    return *this;
  }
  int getFd() const { return this->fd_; }
  const std::string &getFile() const { return this->file_; }
  const bool &getIsAccesslogOn() const { return this->is_accesslog_on_; }
  void setFd(int fd) { this->fd_ = fd; }
  void setFile(const std::string &file) { this->file_ = file; }
  void setIsAccesslogOn(bool is_accesslog_on) { this->is_accesslog_on_ = is_accesslog_on; }
  static const char *kDefaultFile_;
  static const bool kDefaultIsAccesslogOn_ = true;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
