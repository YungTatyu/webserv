#include "conf.hpp"

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <limits>

#include "Lexer.hpp"
#include "LogFd.hpp"
#include "Main.hpp"
#include "Parser.hpp"
#include "error.hpp"
#include "syscall_wrapper.hpp"
#include "utils.hpp"

const char *config::Root::kDefaultPath_ = "html";
const char *config::UseridPath::kDefaultPath_ = "/";
const unsigned long config::ReceiveTimeout::kDefaultTime_ = 60 * Time::seconds;  // 60s
const unsigned long config::SendTimeout::kDefaultTime_ = 60 * Time::seconds;     // 60s
const int config::Return::kRedirectCodes_[] = {301, 302, 303, 307, 308};
const char *config::Listen::kDefaultAddress_ = "127.0.0.1";
const char *config::ServerName::kDefaultName_ = "";
const unsigned long config::Time::kMaxTimeInMilliseconds_ = std::numeric_limits<long>::max();
const unsigned long config::Size::kMaxSizeInBytes_ = std::numeric_limits<long>::max();
const char *config::UseridDomain::kDefaultName_ = "none";
const char *config::AccessLog::kDefaultFile_ = "logs/access.log";
const char *config::ErrorLog::kDefaultFile_ = "logs/error.log";
const unsigned long config::KeepaliveTimeout::kDefaultTime_ = 60 * Time::seconds;  // 60s
const char *config::Index::kDefaultFile_ = "index.html";
const unsigned long config::ClientMaxBodySize::kDefaultSize_;

config::Main *config::initConfig(const std::string &file_path) {
  std::string absolute_path;

  // 絶対pathを取得
  if (!utils::resolvePath(file_path, absolute_path)) {
    error::printError(error::strSysCallError("realpath", utils::quoteStr(file_path)));
    return NULL;
  }

  // file_path が存在するかどうか
  if (syscall_wrapper::Access(absolute_path, F_OK, true) == -1) return NULL;

  // file_path の読み取り権限があるかどうか
  if (syscall_wrapper::Access(absolute_path, R_OK, true) == -1) return NULL;

  // file_path がファイルかどうか確認する。
  if (!utils::isFile(absolute_path, false)) {
    error::printError(utils::quoteStr(absolute_path) + std::string(" is a directory"), config::CRIT);
    return NULL;
  }

  config::Lexer lexer(absolute_path);
  lexer.tokenize();
  const std::vector<Token> &tokens = lexer.getTokens();

  Main *config = new Main();
  config::Parser parser(*config, tokens, absolute_path);
  if (!parser.parse()) {
    delete config;
    return NULL;
  }

  if (!initLogFds(*config)) {
    delete config;
    return NULL;
  }

  return config;
}
