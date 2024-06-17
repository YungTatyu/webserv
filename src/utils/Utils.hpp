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

namespace Utils {
// 大文字小文字を区別しない文字列比較関数
struct CaseInsensitiveCompare {
  bool operator()(std::string lhs, std::string rhs) const {
    std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
    std::transform(rhs.begin(), rhs.end(), rhs.begin(), ::tolower);
    return lhs < rhs;
  }
};

int wrapperOpen(const std::string path, int flags, mode_t modes);
int wrapperAccess(const std::string path, int modes, bool err_log);
bool wrapperRealpath(const std::string& path, std::string& absolute_path);
bool isFile(const std::string& path, bool err_log);
bool isDirectory(const std::string& path, bool err_log);
std::string readFile(const std::string& filePath);
std::vector<std::string> createDirectoryContents(const std::string& directoryPath);
bool isExecutable(const char* filename);
bool isExtensionFile(const std::string& filename, const std::string& extension);
ssize_t wrapperWrite(const int fd, const std::string& msg);
bool wrapperGetsockname(struct sockaddr_in& addr, const int sock);
std::string socketToStrIPAddress(const int sock);
std::string ipToStr(const uint32_t ip);
uint32_t StrToIPAddress(const std::string& ip);
int resolveConnectedPort(const int sock);
std::string toLower(std::string str);
std::string toUpper(std::string str);
bool isSpace(const unsigned char ch);
bool compareIgnoreCase(std::string lhs, std::string rhs);
int setNonBlockingCloExec(const int fd);
size_t strToSizet(const std::string& str);
size_t hexToDec(const std::string& str);
bool isSign(unsigned char ch);
std::string normalizePath(const std::string& full_path);
std::string replace(const std::string& str, char old_c, char new_c);
template <typename T>
std::string toStr(const T value);
bool isNumeric(const std::string& str);
template <typename Context>
bool hasDirective(const Context& context, const std::string& directive);
template <typename T>
T strToT(const std::string& str);
}  // namespace Utils

template <typename T>
std::string Utils::toStr(const T value) {
  std::stringstream converter;
  converter << value;
  return converter.str();
}

template <typename Context>
bool Utils::hasDirective(const Context& context, const std::string& directive) {
  return context.directives_set.find(directive) != context.directives_set.end();
}

template <typename T>
T Utils::strToT(const std::string& str) {
  T t;
  std::istringstream iss(str);
  iss >> t;
  if (iss.fail() || iss.bad() || iss.peek() != EOF) throw std::invalid_argument("strToT");
  return t;
}

#endif
