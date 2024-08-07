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
  std::string err_msg = msg.empty() ? msg : std::string(" ") + msg;  // errorメッセージをフォーマット
  return syscall + "()" + err_msg + " failed (" + utils::toStr(errno) + ": " + std::strerror(errno) + ")";
}

void error::printError(const std::string &msg, config::LOG_LEVEL level) {
  std::cerr << "webserv: [" << config::ErrorLog::LogLevelToStr(level) << "] " << msg << std::endl;
}
