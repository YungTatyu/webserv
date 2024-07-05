#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

#include "ErrorLog.hpp"

namespace error {
std::string strSysCallError(const std::string &syscall, const std::string &msg = "");
void printError(const std::string &msg, config::LOG_LEVEL level = config::EMERG);
}  // namespace error

#endif
