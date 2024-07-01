#ifndef CONFIG_ERRORLOG_HPP
#define CONFIG_ERRORLOG_HPP

#include <string>

#include "conf.hpp"

namespace config {
enum LOG_LEVEL {
  DEBUG = 1,
  INFO = 1 << 1,
  NOTICE = 1 << 2,
  WARN = 1 << 3,
  ERROR = 1 << 4,
  CRIT = 1 << 5,
  ALERT = 1 << 6,
  EMERG = 1 << 7
};
std::string LogLevelToStr(LOG_LEVEL level) {
  switch (level) {
  case DEBUG: return "debug";
  case INFO: return "info";
  case NOTICE: return "notice";
  case WARN: return "warn";
  case ERROR: return "error";
  case CRIT: return "crit";
  case ALERT: return "alert";
  case EMERG: return "emerg";
  default: return "unknown";
  }
}


class ErrorLog {
 private:
  int fd_;
  std::string file_;
  int level_;

 public:
  ErrorLog() :
    fd_(-1),
    file_(this->kDefaultFile_),
    level_(this->kDefaultLevel_) {}
  ErrorLog(const ErrorLog& other) { *this = other; }
  ~ErrorLog() {}
  ErrorLog& operator=(const ErrorLog& other) {
    if (this != &other) {
      this->file_ = other.file_;
    }
    return *this;
  }
  int getFd() const { return this->fd_; }
  void setFd(int fd) { this->fd_ = fd; }
  const std::string& getFile() const { return this->file_; }
  void setFile(const std::string& file) { this->file_ = file; }
  int getLevel() const { return this->level_; }
  void setLevel(int level) { this->level_ = level; }
  static const char* kDefaultFile_;
  static const int kDefaultLevel_;
  static const unsigned int kType_ =
      CONF_MAIN | CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
