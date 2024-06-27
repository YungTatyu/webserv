#ifndef CONFIG_SERVER_NAME_HPP
#define CONFIG_SERVER_NAME_HPP

#include <string>

#include "conf.hpp"

namespace config {
class ServerName {
 private:
  // defaultでは空文字列が入る。
  // セットされた場合、デフォルト値は削除する
  std::set<std::string> name_set_;

 public:
  ServerName() { this->name_set_.insert(this->kDefaultName_); }
  ServerName(const ServerName& other) { *this = other; }
  ~ServerName() {}
  ServerName& operator=(const ServerName& other) {
    if (this != &other) {
      this->name_set_ = other.name_set_;
    }
    return *this;
  }
  const std::set<std::string>& getName() const { return this->name_set_; }
  void addName(const std::string& name) { name_set_.insert(name); }
  void eraseDefaultName() { this->name_set_.erase(kDefaultName_); }
  const static char* kDefaultName_;
  const static unsigned int kType_ = CONF_HTTP_SERVER | CONF_1MORE | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
