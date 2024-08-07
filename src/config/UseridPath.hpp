#ifndef CONFIG_USERID_PATH_HPP
#define CONFIG_USERID_PATH_HPP

#include <string>

#include "conf.hpp"

namespace config {
class UseridPath {
 private:
  std::string path_;

 public:
  UseridPath() : path_(kDefaultPath_) {}
  UseridPath(const UseridPath& other) { *this = other; }
  ~UseridPath() {}
  UseridPath& operator=(const UseridPath& other) {
    if (this != &other) {
      this->path_ = other.path_;
    }
    return *this;
  }
  const std::string& getPath() const { return this->path_; }
  void setPath(const std::string& path) { this->path_ = path; }
  static const char* kDefaultPath_;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
