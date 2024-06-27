#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
namespace error {
std::string strSysCallError(const std::string &syscall, const std::string &msg = "");
}  // namespace error

#endif
