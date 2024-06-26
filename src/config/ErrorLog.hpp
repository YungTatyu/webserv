#ifndef CONFIG_ERRORLOG_HPP
#define CONFIG_ERRORLOG_HPP

#include <string>

#include "conf.hpp"

namespace config {
class ErrorLog {
 private:
  std::string file_;

 public:
  ErrorLog() : file_(this->kDefaultFile_) {}
  ErrorLog(const ErrorLog& other) { *this = other; }
  ~ErrorLog() {}
  ErrorLog& operator=(const ErrorLog& other) {
    if (this != &other) {
      this->file_ = other.file_;
    }
    return *this;
  }
  const std::string& getFile() const { return this->file_; }
  void setFile(const std::string& file) { this->file_ = file; }
  const static char* kDefaultFile_;
  const static unsigned int kType_ =
      CONF_MAIN | CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
