#ifndef LOGFD_HPP
#define LOGFD_HPP

#include <unistd.h>

#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "Main.hpp"

namespace config {

bool initLogFds(Main& config);
bool initAcsLogFds(Main& config);
bool initErrLogFds(Main& config);
int addAcsFdList(std::set<std::string>& directives_set, std::vector<config::AccessLog>& access_log_list);
int addErrFdList(std::set<std::string>& directives_set, std::vector<config::ErrorLog>& error_log_list);
int openLogFd(const std::string& log_path);
bool checkFileAccess(const std::string& path);
template <typename T>
void closeLogFds(const std::vector<T>& log_list);
void terminateLogFds(const Main* config);

}  // namespace config

template <typename T>
void config::closeLogFds(const std::vector<T>& log_list) {
  for (size_t i = 0; i < log_list.size(); i++) {
    close(log_list[i].getFd());
  }
}

#endif
