#include "error.hpp"

#include <cerrno>
#include <cstring>
#include "Utils.hpp"

std::string error::strerror(const std::string &err_msg) {
  const std::string err = "webserv: [emerg] ";
  return err + err_msg + " (" + Utils::toStr(errno) + ":" + std::strerror(errno) + ")";
}
