#ifndef CONFIG_TRY_FILES_HPP
#define CONFIG_TRY_FILES_HPP

#include <string>
#include <vector>

#include "conf.hpp"

namespace config {
class TryFiles {
 private:
  std::vector<std::string> file_list_;
  std::string uri_;  // uri_かcode_どちらか一方しか設定できない
  int code_;         // uri_かcode_どちらか一方しか設定できない
 public:
  TryFiles() : code_(this->kCodeUnset_) {}
  TryFiles(const TryFiles &other) { *this = other; }
  ~TryFiles() {}
  TryFiles &operator=(const TryFiles &other) {
    if (this != &other) {
      this->file_list_ = other.file_list_;
      this->uri_ = other.uri_;
      this->code_ = other.code_;
    }
    return *this;
  }
  const std::vector<std::string> &getFileList() const { return this->file_list_; }
  const std::string &getUri() const { return this->uri_; }
  const int &getCode() const { return this->code_; }
  void addFile(const std::string &file) { this->file_list_.push_back(file); }
  void setUri(const std::string &uri) { this->uri_ = uri; }
  void setCode(int code) { this->code_ = code; }
  const static int kCodeUnset_ = -1;
  const static unsigned int kType_ = CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_2MORE | CONF_UNIQUE;
};
}  // namespace config

#endif
