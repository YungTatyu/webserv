#ifndef UTILS_HPP
#define UTILS_HPP

#include <dirent.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <ctime>

namespace utils {
// 大文字小文字を区別しない文字列比較関数
struct CaseInsensitiveCompare {
  bool operator()(std::string lhs, std::string rhs) const {
    std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
    std::transform(rhs.begin(), rhs.end(), rhs.begin(), ::tolower);
    return lhs < rhs;
  }
};

bool resolvePath(const std::string& path, std::string& absolute_path);
bool isFile(const std::string& path, bool err_log);
bool isDirectory(const std::string& path, bool err_log);
std::string readFile(const std::string& path);
std::vector<std::string> createDirectoryContents(const std::string& path);
bool isExecutable(const char* filename);
bool isExtensionFile(const std::string& filename, const std::string& extension);
ssize_t writeChunks(int fd, const std::string& msg);
bool resolveSocketAddr(struct sockaddr_in& addr, int sock);
std::string socketToStrIPAddress(int sock);
std::string ipToStr(uint32_t ip);
uint32_t strToIPAddress(const std::string& ip);
int resolveConnectedPort(int sock);
std::string toLower(std::string str);
std::string toUpper(std::string str);
bool isSpace(unsigned char ch);
bool compareIgnoreCase(std::string lhs, std::string rhs);
int setNonBlockCloExec(int fd);
size_t strToSizet(const std::string& str);
size_t hexToDec(const std::string& str);
bool isSign(unsigned char ch);
std::string normalizePath(const std::string& full_path);
std::string replace(const std::string& str, char old_c, char new_c);
std::string formatTm(const std::tm* timeinfo, const std::string& format);
template <typename T>
std::string toStr(T value);
bool isNumeric(const std::string& str);
template <typename Context>
bool hasDirective(const Context& context, const std::string& directive);
template <typename T>
T strToT(const std::string& str);
}  // namespace utils

template <typename T>
std::string utils::toStr(T value) {
  std::stringstream converter;
  converter << value;
  return converter.str();
}

template <typename Context>
bool utils::hasDirective(const Context& context, const std::string& directive) {
  return context.directives_set_.find(directive) != context.directives_set_.end();
}

template <typename T>
T utils::strToT(const std::string& str) {
  T t;
  std::istringstream iss(str);
  iss >> t;
  if (iss.fail() || iss.bad() || iss.peek() != EOF) throw std::invalid_argument("strToT");
  return t;
}

#endif
