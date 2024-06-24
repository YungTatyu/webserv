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
  std::set<std::string> directives_set_;
  std::vector<ErrorLog> error_log_list_;
  Events events_;
  Http http_;
  std::vector<int> error_fd_list_;
};

}  // namespace config

#endif
