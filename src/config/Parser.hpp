#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <sys/types.h>

#include <cstddef>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Main.hpp"
#include "conf.hpp"
#include "utils.hpp"

namespace config {
enum OS {
  OS_BSD_BASED,
  OS_LINUX,
  OS_OTHER
};

class Parser {
 private:
  Parser();
  Parser(const Parser &other);
  Parser &operator=(const Parser &other);
  bool parseType(const Token &token);
  bool expectTokenType(config::TK_TYPE type, const Token &token) const;
  bool expectArgsNum(unsigned int expect, unsigned int actual) const;
  bool expectTerminatingToken() const;
  bool isContext(const Token &token) const;
  bool isDirective(const Token &token) const;
  const std::set<std::string> *findDirectivesSet(CONTEXT context) const;
  void printError(const std::string &err_msg, const Token &token) const;
  void printFormatedError(const std::string &message, const Token &token) const;
  size_t countArgs(TK_TYPE terminating_token) const;
  bool validFinalState() const;
  bool validWorkerConnections() const;
  template <typename T>
  void updateContext(T &context, CONTEXT new_context, const std::string &context_name);
  bool parseDirective(bool (config::Parser::*directive_parser)());
  bool parseHttpServerEvents();
  bool parseLocation();
  bool parseLimitExcept();
  bool parseAccessLog();
  bool parseAlias();
  bool parseAllowDeny();
  bool parseAutoindex();
  bool parseClientMaxBodySize();
  bool parseErrorLog();
  bool parseErrorPage();
  bool parseIndex();
  bool parseKeepaliveTimeout();
  bool parseListen();
  bool parseReceiveTimeout();
  bool parseReturn();
  bool parseRoot();
  bool parseSendTimeout();
  bool parseServerName();
  bool parseTryFiles();
  bool parseUse();
  bool parseUserid();
  bool parseUseridDomain();
  bool parseUseridExpires();
  bool parseUseridPath();
  bool parseUseridService();
  bool parseWorkerConnections();
  bool isIPv4(const std::string &ipv4) const;
  bool isIPv6(const std::string &ipv6) const;
  bool isMixedIPAddress(const std::string &ipv6) const;
  bool isNumInRange(const std::string &num, long min, long max) const;
  bool canConvertMinTime(long &value, const std::string &unit);
  bool canConvertMinSize(long &value, const std::string &unit);
  bool validEventType(const std::string &eventType) const;
  std::pair<long, std::string> parseValueWithUnit(const std::set<std::string> &units) const;
  long parseTime();
  long parseSize();
  unsigned int retCodeIfValid();
  long retErrorPageOptNumIfValid();
  REQUEST_METHOD convertToRequestMethod(const std::string &method) const;
  std::string toUpper(std::string) const;
  bool isDuplicateDefaultServer(const config::Listen &this_listen);
  void updateDirectivesSet(const std::string &directive);
  Main &config_;
  const std::vector<Token> &tokens_;
  const std::string filepath_;
  size_t ti_;  // token index
  std::string current_directive_;
  std::stack<CONTEXT> current_context_;
  std::map<std::string, bool (config::Parser::*)()> parser_map_;

 public:
  Parser(Main &config, const std::vector<Token> &tokens, const std::string &filepath);
  ~Parser();
  static std::map<std::string, unsigned int> all_directives_;  // すべてのcontextとdirectiveの情報を管理
  bool parse();
  const Main &getConfig() const;
};
}  // namespace config

/**
 * current_contextを更新し、directives_setにもcontextを追加する。
 *
 */
template <typename T>
void config::Parser::updateContext(T &context, CONTEXT new_context, const std::string &context_name) {
  current_context_.push(new_context);
  context.directives_set_.insert(context_name);
}

#endif
