#include "utils.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>

#include "WebServer.hpp"
#include "error.hpp"
#include "syscall_wrapper.hpp"

bool utils::resolvePath(const std::string& path, std::string& absolute_path) {
  char tmp_path[MAXPATHLEN];
  if (realpath(path.c_str(), tmp_path) == NULL) {
    return false;
  }

  absolute_path = static_cast<std::string>(tmp_path);
  return true;
}

bool utils::isFile(const std::string& path, bool err_log) {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) {
    if (err_log) WebServer::writeErrorlog(error::strSysCallError("stat", path) + "\n", config::INFO);
    return false;
  }
  return S_ISREG(statbuf.st_mode);
}

bool utils::isDirectory(const std::string& path, bool err_log) {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) {
    if (err_log) WebServer::writeErrorlog(error::strSysCallError("stat", path) + "\n", config::INFO);
    return false;
  }
  return S_ISDIR(statbuf.st_mode);
}

std::string utils::readFile(const std::string& path) {
  std::ifstream file(path.c_str());
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::vector<std::string> utils::createDirectoryContents(const std::string& path) {
  std::vector<std::string> contents;
  DIR* dir = opendir(path.c_str());
  // error出力？
  if (dir == NULL) return contents;
  struct dirent* entry;

  contents.push_back("../");

  while ((entry = readdir(dir)) != NULL) {
    std::string filename = entry->d_name;
    if (filename != "." && filename != "..") {
      if (utils::isDirectory(path + "/" + filename, false)) filename += "/";
      contents.push_back(filename);
    }
  }
  closedir(dir);

  return contents;
}

bool utils::isExecutable(const char* filename) {
  struct stat sbuf;
  if (stat(filename, &sbuf) < 0) return false;
  return S_ISREG(sbuf.st_mode) && (S_IXUSR & sbuf.st_mode);
}

bool utils::isExtensionFile(const std::string& filename, const std::string& extension) {
  if (filename.length() < extension.length()) return false;
  return std::equal(extension.begin(), extension.end(), filename.end() - extension.length());
}

ssize_t utils::writeChunks(int fd, const std::string& msg) {
  size_t msg_size = msg.size();
  size_t written_bytes = 0;
  size_t chunksize = 1024;

  while (written_bytes < msg_size) {
    size_t write_size = std::min(chunksize, msg_size - written_bytes);
    std::string chunk = msg.substr(written_bytes, write_size);

    ssize_t ret = write(fd, chunk.c_str(), chunk.size());
    if (ret == -1) return -1;

    written_bytes += ret;
  }
  return written_bytes;
}

bool utils::resolveSocketAddr(struct sockaddr_in& addr, int sock) {
  socklen_t client_addrlen = sizeof(addr);
  if (getsockname(sock, reinterpret_cast<struct sockaddr*>(&addr), &client_addrlen) == -1) {
    WebServer::writeErrorlog(error::strSysCallError("getsockname", utils::toStr(sock)) + "\n", config::WARN);
    return false;
  }
  return true;
}

/**
 * @brief clientがリクエストを送ったサーバーのport番号を返す
 *
 * @param sock
 * @return int
 */
int utils::resolveConnectedPort(int sock) {
  struct sockaddr_in addr;
  if (!resolveSocketAddr(addr, sock)) return -1;
  return ntohs(addr.sin_port);
}

/**
 * @brief socketからipアドレスの文字列を作成する
 *
 * @param sock
 * @return std::string
 */
std::string utils::socketToStrIPAddress(int sock) {
  struct sockaddr_in addr;
  if (!resolveSocketAddr(addr, sock)) return "";
  return ipToStr(addr.sin_addr.s_addr);
}

std::string utils::ipToStr(uint32_t ip) {
  std::stringstream ss;
  uint32_t ip_host_order = ntohl(ip);  // ネットワークバイト順からホストバイト順に変換
  ss << ((ip_host_order >> 24) & 0xFF) << '.'  // 第1オクテット
     << ((ip_host_order >> 16) & 0xFF) << '.'  // 第2オクテット
     << ((ip_host_order >> 8) & 0xFF) << '.'   // 第3オクテット
     << (ip_host_order & 0xFF);                // 第4オクテット
  return ss.str();
}

uint32_t utils::strToIPAddress(const std::string& ip) {
  std::istringstream iss(ip);
  std::string segment;
  std::vector<std::string> segments;

  // "." で分割
  while (std::getline(iss, segment, '.')) {
    segments.push_back(segment);
  }

  uint32_t result = 0;

  for (int i = 0; i < 4; i++) {
    iss.clear();
    iss.str(segments[i]);
    int value;
    iss >> value;

    result = (result << 8) | value;
  }

  return result;
}

std::string utils::toLower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

std::string utils::toUpper(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

bool utils::isSpace(unsigned char ch) { return ch == ' '; }

/**
 * @brief 文字列の大文字小文字の差異を無視して、比較する
 *
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
bool utils::compareIgnoreCase(std::string lhs, std::string rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
  std::transform(rhs.begin(), rhs.end(), rhs.begin(), ::tolower);
  return lhs == rhs;
}

/**
 * @brief fdに以下を設定する
 *
 * 1: non-blocking
 * 2: exec()実行直前にfdをcloseする
 *
 * @param fd
 * @return int
 */
int utils::setNonBlockCloExec(int fd) {
  int nonblock = syscall_wrapper::Fcntl(fd, F_SETFL, O_NONBLOCK);
  // 以下はサブジェクトで使えないフラグ使用
  int closex = syscall_wrapper::Fcntl(fd, F_SETFD, FD_CLOEXEC);
  if (nonblock == -1 || closex == -1) return -1;
  return closex;
}

size_t utils::strToSizet(const std::string& str) {
  std::istringstream iss(str);
  size_t size;
  iss >> size;
  return size;
}

size_t utils::hexToDec(const std::string& str) {
  std::istringstream iss(str);
  size_t size;
  iss >> std::hex >> size;
  return size;
}

bool utils::isSign(unsigned char ch) { return ch == '+' || ch == '-'; }

bool utils::isNumeric(const std::string& str) {
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
    if (!std::isdigit(*it)) return false;
  }
  return true;
}

/**
 * @brief 絶対パスを正規化する
 * WARNING:相対パスは引数で渡さない
 */
std::string utils::normalizePath(const std::string& full_path) {
  std::vector<std::string> components;
  std::istringstream ss(full_path);
  std::string token;

  if (full_path.empty()) return "/";
  bool end_with_sl = full_path.at(full_path.size() - 1) == '/' ? true : false;
  while (std::getline(ss, token, '/')) {
    if (token.empty() || token == ".") {
      continue;
    }
    if (token == "..") {
      if (!components.empty()) components.pop_back();
      continue;
    }
    components.push_back(token);
  }

  std::string result = "/";
  for (size_t i = 0; i < components.size(); ++i) {
    if (i != 0) result += "/";
    result += components[i];
  }
  if (end_with_sl && result != "/") result += "/";
  return result;
}

std::string utils::replace(const std::string& str, char old_c, char new_c) {
  std::string new_str;
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
    if (*it == old_c) {
      new_str += new_c;
      continue;
    }
    new_str += *it;
  }
  return new_str;
}

// std::tmを指定されたフォーマットの文字列に変換する関数
std::string utils::formatTm(const std::tm* timeinfo, const std::string& format) {
    char buffer[1024];
    std::strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
    return std::string(buffer);
}
