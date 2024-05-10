#include "error.hpp"

#include <cerrno>
#include <cstring>

#include "Utils.hpp"

std::string error::strSysCallError(const std::string &syscall) {
  const std::string err = "webserv: [emerg] ";
  return err + syscall + "() failed (" + Utils::toStr(errno) + ":" + std::strerror(errno) + ")";
}
