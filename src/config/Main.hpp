#ifndef CONFIG_MAIN_HPP
#define CONFIG_MAIN_HPP

#include <set>
#include <string>
#include <vector>

#include "ErrorLog.hpp"
#include "Events.hpp"
#include "Http.hpp"

namespace config {
struct Main {
  Main() {}
  Main(const Main& other) { *this = other; }
  ~Main() {}
  Main& operator=(const Main& other) {
    if (this != &other) {
      this->directives_set_ = other.directives_set_;
      this->error_log_list_ = other.error_log_list_;
      this->events_ = other.events_;
      this->http_ = other.http_;
      this->error_fd_list_ = other.error_fd_list_;
    }
    return *this;
  }
  std::set<std::string> directives_set_;
  std::vector<ErrorLog> error_log_list_;
  Events events_;
  Http http_;
  std::vector<int> error_fd_list_;
};

}  // namespace config

#endif
