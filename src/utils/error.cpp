#include "error.hpp"

#include <cerrno>
#include <cstring>

#include "utils.hpp"

/**
 * @brief errorメッセージの文字列を取得する
 *
 * @param syscall
 * @param msg 初めの空白も含めたメッセージにすること
 * @return std::string
 */
std::string error::strSysCallError(const std::string &syscall, const std::string &msg) {
  std::string err = "webserv: [emerg] ";
  std::string err_msg = msg.empty() ? msg : std::string(" ") + msg;  // errorメッセージをフォーマット
  return err + syscall + "()" + err_msg + " failed (" + utils::toStr(errno) + ": " + std::strerror(errno) +
         ")";
}
