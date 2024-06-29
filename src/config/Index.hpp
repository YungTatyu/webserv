#ifndef CONFIG_INDEX_HPP
#define CONFIG_INDEX_HPP

#include <string>

#include "conf.hpp"

namespace config {
class Index {
 private:
  std::string file_;

 public:
  Index() : file_(this->kDefaultFile_) {}
  Index(const Index &other) { *this = other; }
  ~Index() {}
  Index &operator=(const Index &other) {
    if (this != &other) {
      this->file_ = other.file_;
    }
    return *this;
  }
  const std::string &getFile() const { return this->file_; }
  void setFile(const std::string &file) { this->file_ = file; }
  static const char *kDefaultFile_;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_1MORE | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
