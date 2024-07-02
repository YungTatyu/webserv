#include "Parser.hpp"

#include <algorithm>
#include <climits>
#include <iostream>
#include <sstream>
#include <utility>

std::map<std::string, unsigned int> config::Parser::all_directives_;

const unsigned int config::Http::kType_;
const unsigned int config::Events::kType_;
const unsigned int config::Server::kType_;
const unsigned int config::Location::kType_;
const unsigned int config::LimitExcept::kType_;
const unsigned int config::AccessLog::kType_;
const unsigned int config::Alias::kType_;
const unsigned int config::AllowDeny::kType_;
const unsigned int config::Autoindex::kType_;
const unsigned int config::ClientMaxBodySize::kType_;
const unsigned int config::ErrorLog::kType_;
const unsigned int config::ErrorPage::kType_;
const unsigned int config::Index::kType_;
const unsigned int config::KeepaliveTimeout::kType_;
const unsigned int config::Listen::kType_;
const unsigned int config::ReceiveTimeout::kType_;
const unsigned int config::Return::kType_;
const unsigned int config::Root::kType_;
const unsigned int config::SendTimeout::kType_;
const unsigned int config::ServerName::kType_;
const unsigned int config::TryFiles::kType_;
const unsigned int config::Use::kType_;
const unsigned int config::Userid::kType_;
const unsigned int config::UseridDomain::kType_;
const unsigned int config::UseridExpires::kType_;
const unsigned int config::UseridPath::kType_;
const unsigned int config::UseridService::kType_;
const unsigned int config::WorkerConnections::kType_;

static const std::string kHttp = "http";
static const std::string kEvents = "events";
static const std::string kServer = "server";
static const std::string kLocation = "location";
static const std::string kLimitExcept = "limit_except";
static const std::string kAccessLog = "access_log";
static const std::string kAlias = "alias";
static const std::string kAllow = "allow";
static const std::string kAutoindex = "autoindex";
static const std::string kClientMaxBodySize = "client_max_body_size";
static const std::string kDeny = "deny";
static const std::string kErrorLog = "error_log";
static const std::string kErrorPage = "error_page";
static const std::string kIndex = "index";
static const std::string kKeepaliveTimeout = "keepalive_timeout";
static const std::string kListen = "listen";
static const std::string kReceiveTimeout = "receive_timeout";
static const std::string kReturn = "return";
static const std::string kRoot = "root";
static const std::string kSendTimeout = "send_timeout";
static const std::string kServerName = "server_name";
static const std::string kTryFiles = "try_files";
static const std::string kUse = "use";
static const std::string kUserid = "userid";
static const std::string kUseridDomain = "userid_domain";
static const std::string kUseridExpires = "userid_expires";
static const std::string kUseridPath = "userid_path";
static const std::string kUseridService = "userid_service";
static const std::string kWorkerConnections = "worker_connections";
static const std::string kSelect = "select";
static const std::string kPoll = "poll";
static const std::string kEpoll = "epoll";
static const std::string kKqueue = "kqueue";

config::Parser::Parser(Main &config, const std::vector<Token> &tokens, const std::string &filepath)
    : config_(config), tokens_(tokens), filepath_(filepath), ti_(0), current_directive_("") {
  // 現在のcontextをセット
  this->current_context_.push(CONF_MAIN);

  // context
  this->all_directives_.insert(std::make_pair(kEvents, config::Events::kType_));
  this->all_directives_.insert(std::make_pair(kHttp, config::Http::kType_));
  this->all_directives_.insert(std::make_pair(kServer, config::Server::kType_));
  this->all_directives_.insert(std::make_pair(kLocation, config::Location::kType_));
  this->all_directives_.insert(std::make_pair(kLimitExcept, config::LimitExcept::kType_));

  // directive
  this->all_directives_.insert(std::make_pair(kAccessLog, config::AccessLog::kType_));
  this->all_directives_.insert(std::make_pair(kAlias, config::Alias::kType_));
  this->all_directives_.insert(std::make_pair(kAllow, config::AllowDeny::kType_));
  this->all_directives_.insert(std::make_pair(kAutoindex, config::Autoindex::kType_));
  this->all_directives_.insert(std::make_pair(kClientMaxBodySize, config::ClientMaxBodySize::kType_));
  this->all_directives_.insert(std::make_pair(kDeny, config::AllowDeny::kType_));
  this->all_directives_.insert(std::make_pair(kErrorLog, config::ErrorLog::kType_));
  this->all_directives_.insert(std::make_pair(kErrorPage, config::ErrorPage::kType_));
  this->all_directives_.insert(std::make_pair(kIndex, config::Index::kType_));
  this->all_directives_.insert(std::make_pair(kKeepaliveTimeout, config::KeepaliveTimeout::kType_));
  this->all_directives_.insert(std::make_pair(kListen, config::Listen::kType_));
  this->all_directives_.insert(std::make_pair(kReceiveTimeout, config::ReceiveTimeout::kType_));
  this->all_directives_.insert(std::make_pair(kReturn, config::Return::kType_));
  this->all_directives_.insert(std::make_pair(kRoot, config::Root::kType_));
  this->all_directives_.insert(std::make_pair(kSendTimeout, config::SendTimeout::kType_));
  this->all_directives_.insert(std::make_pair(kServerName, config::ServerName::kType_));
  this->all_directives_.insert(std::make_pair(kTryFiles, config::TryFiles::kType_));
  this->all_directives_.insert(std::make_pair(kUse, config::Use::kType_));
  // this->all_directives_.insert(std::make_pair(kUserid, config::Userid::kType_));
  // this->all_directives_.insert(std::make_pair(kUseridDomain, config::UseridDomain::kType_));
  // this->all_directives_.insert(std::make_pair(kUseridExpires, config::UseridExpires::kType_));
  // this->all_directives_.insert(std::make_pair(kUseridPath, config::UseridPath::kType_));
  // this->all_directives_.insert(std::make_pair(kUseridService, config::UseridService::kType_));
  this->all_directives_.insert(std::make_pair(kWorkerConnections, config::WorkerConnections::kType_));

  // parser
  this->parser_map_[kHttp] = &config::Parser::parseHttpServerEvents;
  this->parser_map_[kEvents] = &config::Parser::parseHttpServerEvents;
  this->parser_map_[kServer] = &config::Parser::parseHttpServerEvents;
  this->parser_map_[kLocation] = &config::Parser::parseLocation;
  this->parser_map_[kLimitExcept] = &config::Parser::parseLimitExcept;

  this->parser_map_[kAccessLog] = &config::Parser::parseAccessLog;
  this->parser_map_[kAlias] = &config::Parser::parseAlias;
  this->parser_map_[kAllow] = &config::Parser::parseAllowDeny;
  this->parser_map_[kAutoindex] = &config::Parser::parseAutoindex;
  this->parser_map_[kClientMaxBodySize] = &config::Parser::parseClientMaxBodySize;
  this->parser_map_[kDeny] = &config::Parser::parseAllowDeny;
  this->parser_map_[kErrorLog] = &config::Parser::parseErrorLog;
  this->parser_map_[kErrorPage] = &config::Parser::parseErrorPage;
  this->parser_map_[kIndex] = &config::Parser::parseIndex;
  this->parser_map_[kKeepaliveTimeout] = &config::Parser::parseKeepaliveTimeout;
  this->parser_map_[kListen] = &config::Parser::parseListen;
  this->parser_map_[kReceiveTimeout] = &config::Parser::parseReceiveTimeout;
  this->parser_map_[kRoot] = &config::Parser::parseRoot;
  this->parser_map_[kSendTimeout] = &config::Parser::parseSendTimeout;
  this->parser_map_[kReturn] = &config::Parser::parseReturn;
  this->parser_map_[kServerName] = &config::Parser::parseServerName;
  this->parser_map_[kTryFiles] = &config::Parser::parseTryFiles;
  this->parser_map_[kUse] = &config::Parser::parseUse;
  // this->parser_map_[kUserid] = &config::Parser::parseUserid;
  // this->parser_map_[kUseridDomain] = &config::Parser::parseUseridDomain;
  // this->parser_map_[kUseridExpires] = &config::Parser::parseUseridExpires;
  // this->parser_map_[kUseridPath] = &config::Parser::parseUseridPath;
  // this->parser_map_[kUseridService] = &config::Parser::parseUseridService;
  this->parser_map_[kWorkerConnections] = &config::Parser::parseWorkerConnections;
}

config::Parser::~Parser() {}

/**
 * parseの流れ
 * 1. directiveが終了しているか: ";", "{"
 * 2. 存在するdirectiveか
 * 3. contextが正しいか
 * 4. argsの数が正しいか
 * 5. 重複を確認
 * 6. argsの値
 */
bool config::Parser::parse() {
  while (1) {
    const Token &current_token = this->tokens_[ti_];
    if (current_token.type_ == TK_END) break;

    // "}" tokenの場合、tokenを進める
    if (current_token.type_ == TK_CLOSE_CURLY_BRACE) {
      // main contextでは "}" はエラー
      if (this->current_context_.top() == CONF_MAIN) {
        printError(std::string("unexpected \"") + current_token.value_ + "\"", current_token);
        return false;
      }
      this->current_context_.pop();
      ++ti_;
      continue;
    }

    // meta charではない
    if (!expectTokenType(TK_STR, current_token)) return false;

    // ";", "{"が存在するはず
    if (!expectTerminatingToken()) return false;

    // 存在するcontextまたはdirectiveか
    if (!(isDirective(current_token) || isContext(current_token))) {
      printFormatedError("unknown directive", current_token);
      return false;
    }

    // directiveのtypeを確認
    if (!parseType(current_token)) return false;

    this->current_directive_ = current_token.value_;
    // directiveのargsの値を確認
    bool (config::Parser::*directive_parser)() = this->parser_map_[current_directive_];
    if (!parseDirective(directive_parser)) return false;
  }

  return validFinalState();
}

/**
 * 1. contextが正しいか確認
 * 2. argsの数が正しいか確認
 * 3. 重複を確認
 */
bool config::Parser::parseType(const Token &token) {
  const std::string directive_name = token.value_;
  // contextが正しいか
  if (!(this->all_directives_[directive_name] & this->current_context_.top())) {
    printError(std::string("\"") + directive_name + "\" directive is not allowed here", token);
    return false;
  }

  // argsの数が正しいか
  const TK_TYPE terminating_token = isContext(token) ? TK_OPEN_CURLY_BRACE : TK_SEMICOLON;
  const size_t args_num = countArgs(terminating_token);
  bool ret;
  switch (args_num) {
    case 0:
      ret = expectArgsNum(CONF_NOARGS, this->all_directives_[directive_name]);
      break;
    case 1:
      ret = expectArgsNum(CONF_TAKE1 | CONF_1MORE, this->all_directives_[directive_name]);
      break;
    case 2:
      ret = expectArgsNum(CONF_TAKE2 | CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    case 3:
      ret = expectArgsNum(CONF_TAKE3 | CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    case 4:
      ret = expectArgsNum(CONF_TAKE4 | CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    case 5:
      ret = expectArgsNum(CONF_TAKE5 | CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    case 6:
      ret = expectArgsNum(CONF_TAKE6 | CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    case 7:
      ret = expectArgsNum(CONF_TAKE7 | CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    case 8:
      ret = expectArgsNum(CONF_1MORE | CONF_2MORE, this->all_directives_[directive_name]);
      break;
    default:
      ret = false;
      break;
  }
  if (!ret) {
    printError(std::string("invalid number of arguments in \"") + directive_name + "\" directive", token);
    return false;
  }

  // 重複を確認
  const std::set<std::string> *directives_set = findDirectivesSet(this->current_context_.top());
  // directiveが重複不可かつ重複していたらエラー
  if (directives_set != NULL && (this->all_directives_[directive_name] & CONF_UNIQUE) &&
      directives_set->find(directive_name) != directives_set->end()) {
    printError(std::string("\"" + token.value_ + "\" directive is duplicate"), token);
    return false;
  }
  return true;
}

const std::set<std::string> *config::Parser::findDirectivesSet(CONTEXT context) const {
  const std::set<std::string> *ret = NULL;
  switch (context) {
    case CONF_MAIN:
      ret = &(this->config_.directives_set_);
      break;

    case CONF_HTTP:
      ret = &(this->config_.http_.directives_set_);
      break;

    case CONF_EVENTS:
      ret = &(this->config_.events_.directives_set_);
      break;

    case CONF_HTTP_SERVER: {
      const std::vector<Server> &server_list = this->config_.http_.server_list_;
      // serverがすでに存在している場合は、一番最後にparseしたserverのset_directiveを取得
      ret = server_list.size() != 0 ? &(server_list.back().directives_set_) : NULL;
    } break;

    case CONF_HTTP_LOCATION: {
      const Server &current_server = this->config_.http_.server_list_.back();
      const std::vector<Location> &location_list = current_server.location_list_;
      // locationがすでに存在している場合は、一番最後にparseしたlocationのset_directiveを取得
      ret = location_list.size() != 0 ? &(location_list.back().directives_set_) : NULL;
    } break;

    case CONF_HTTP_LIMIT_EXCEPT: {
      const LimitExcept &current_limit_except =
          this->config_.http_.server_list_.back().location_list_.back().limit_except_;
      ret = &(current_limit_except.directives_set_);
    } break;

    default:
      ret = NULL;
      break;
  }
  return ret;
}

bool config::Parser::expectTokenType(config::TK_TYPE type, const Token &token) const {
  if (type != token.type_) {
    printFormatedError("unexpected", token);
    return false;
  }
  return true;
}

/**
 * expectのbitがactualにも立っているか確認
 *
 * 以下の場合はtrue:
 * expect: 0100
 * actual: 0111
 *
 * 以下の場合はfalse
 * expect: 0100
 * actual: 1011
 *
 */
bool config::Parser::expectArgsNum(unsigned int expect, unsigned int actual) const { return expect & actual; }

/**
 * directiveが ";"もしくは"{"で区切られているか確認
 */
bool config::Parser::expectTerminatingToken() const {
  size_t i = this->ti_ + 1;

  while (this->tokens_[i].type_ != TK_SEMICOLON && this->tokens_[i].type_ != TK_OPEN_CURLY_BRACE) {
    if (this->tokens_[i].type_ == TK_END) {
      printError("unexpected end of file, expecting \";\" or \"}\"", this->tokens_[i]);
      return false;
    }
    if (!expectTokenType(TK_STR, this->tokens_[i])) return false;
    ++i;
  }
  return true;
}

/**
 * main contextはconfで設定されないため、含めない
 */
bool config::Parser::isContext(const config::Token &token) const {
  return token.type_ == config::TK_STR &&
         (token.value_ == kEvents || token.value_ == kHttp || token.value_ == kServer ||
          token.value_ == kLocation || token.value_ == kLimitExcept);
}

bool config::Parser::isDirective(const config::Token &token) const {
  return (token.type_ == config::TK_STR && !isContext(token) &&
          this->all_directives_.find(token.value_) != this->all_directives_.end());
}

/**
 * tokenを一時的に進める必要があるため、引数でtokenを渡さない
 * @param terminating_token: directive、contextの終了条件
 * directive ;
 * context   {
 *
 */
size_t config::Parser::countArgs(TK_TYPE terminating_token) const {
  size_t i = this->ti_ + 1;
  size_t args_num = 0;

  while (this->tokens_[i].type_ != terminating_token) {
    ++i;
    ++args_num;
  }
  return args_num;
}

void config::Parser::printFormatedError(const std::string &message, const Token &token) const {
  printError(message + " \"" + token.value_ + "\"", token);
}

void config::Parser::printError(const std::string &err_msg, const Token &token) const {
  std::cerr << "webserv: [emerg] " << err_msg << " in " + this->filepath_ << ":" << token.line_ << '\n';
}

/**
 * parse終了後の状態が正しいか確認する関数。
 *
 */
bool config::Parser::validFinalState() const {
  // current contextがmainでないとerror
  if (this->current_context_.top() != CONF_MAIN) {
    printError("unexpected end of file, expecting \"}\"", this->tokens_[ti_]);
    return false;
  }
  // events contextが設定されていないとerror
  if (!utils::hasDirective(this->config_, kEvents)) {
    std::cerr << "webserv: [emerg] no \"events\" section in configuration\n";
    return false;
  }
  return validWorkerConnections();
}

bool config::Parser::validWorkerConnections() const {
  // use directiveにselectが設定されている場合にのみ最大値を超えていないか確認
  if (this->config_.events_.use_.getConnectionMethod() == config::SELECT &&
      this->config_.events_.worker_connections_.getWorkerConnections() >
          config::WorkerConnections::kSelectMaxConnections) {
    std::cerr << "webserv: [emerg] the maximum number of files supported by select() is "
              << config::WorkerConnections::kSelectMaxConnections << std::endl;
    return false;
  }
  return true;
}

/**
 * すべてのdirective(contextも含めて)のparserをこの関数を通して呼ぶ
 *
 */
bool config::Parser::parseDirective(bool (config::Parser::*directive_parser)()) {
  // contextの場合
  if (isContext(this->tokens_[ti_])) return (this->*directive_parser)();

  // directiveの場合
  ++ti_;  // argumentsに進む
  bool result = (this->*directive_parser)();
  if (!result) return false;
  if (!current_directive_.empty()) updateDirectivesSet(current_directive_);
  ti_ += 2;  // セミコロンを飛ばして次のdirectiveに進む
  return result;
}

/**
 * http, server, eventsをparse
 */
bool config::Parser::parseHttpServerEvents() {
  const std::vector<Token> &tokens = this->tokens_;
  const std::string &context = tokens[ti_].value_;

  ++ti_;  // tokenをcontextの引数に進める

  // current contextをupdate
  if (context == kHttp) {
    updateContext(this->config_, CONF_HTTP, kHttp);
  } else if (context == kServer) {
    // 新たなserver contextを追加
    this->config_.http_.server_list_.push_back(Server());
    updateContext(this->config_.http_, CONF_HTTP_SERVER, kServer);
  } else if (context == kEvents) {
    updateContext(this->config_, CONF_EVENTS, kEvents);
  }

  ++ti_;  // 次のtokenに進める
  return true;
}

bool config::Parser::parseLocation() {
  const std::vector<Token> &tokens = this->tokens_;
  ++ti_;  // tokenをcontextの引数に進める

  // locationのuriが重複していないか確認
  const std::string &uri = tokens[ti_].value_;
  std::vector<Location> &list = this->config_.http_.server_list_.back().location_list_;
  for (std::vector<Location>::iterator it = list.begin(); it != list.end(); ++it) {
    if (it->uri_ == uri) {
      printFormatedError("duplicate location", tokens[ti_]);
      return false;
    }
  }
  list.push_back(Location(uri));

  updateContext(this->config_.http_.server_list_.back(), CONF_HTTP_LOCATION, kLocation);

  ti_ += 2;  // "{" を飛ばして、次のtokenへ進む
  return true;
}

bool config::Parser::parseLimitExcept() {
  const std::vector<Token> &tokens = this->tokens_;
  std::set<REQUEST_METHOD> &excepted_methods =
      this->config_.http_.server_list_.back().location_list_.back().limit_except_.excepted_methods_;
  ++ti_;  // tokenをcontextの引数に進める
  do {
    const std::string upper_case_method = toUpper(tokens[ti_].value_);
    if (!(upper_case_method == "GET" || upper_case_method == "HEAD" || upper_case_method == "POST" ||
          upper_case_method == "DELETE")) {
      printFormatedError("invalid method", tokens[ti_]);
      return false;
    }
    const REQUEST_METHOD method = convertToRequestMethod(upper_case_method);
    excepted_methods.insert(method);
    ++ti_;
  } while (tokens[ti_].type_ != TK_OPEN_CURLY_BRACE);

  updateContext(this->config_.http_.server_list_.back().location_list_.back(), CONF_HTTP_LIMIT_EXCEPT,
                kLimitExcept);

  ++ti_;
  return true;
}

config::REQUEST_METHOD config::Parser::convertToRequestMethod(const std::string &method) const {
  REQUEST_METHOD ret = GET;

  if (method == "GET")
    ret = GET;
  else if (method == "HEAD")
    ret = HEAD;
  else if (method == "POST")
    ret = POST;
  else if (method == "DELETE")
    ret = DELETE;
  return ret;
}

std::string config::Parser::toUpper(std::string str) const {
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    *it = std::toupper(*it);
  }
  return str;
}

bool config::Parser::parseAccessLog() {
  std::string path = this->tokens_[ti_].value_;
  config::CONTEXT context = this->current_context_.top();
  config::AccessLog tmp_acs_log;

  // 文字列が空の場合はobjectを追加しない
  if (path.empty()) {
    // directive_setに追加しない。
    this->current_directive_.clear();
    return true;
  }

  if (path == "off")
    tmp_acs_log.setIsAccesslogOn(false);
  else
    tmp_acs_log.setFile(path);

  if (context == config::CONF_HTTP)
    this->config_.http_.access_log_list_.push_back(tmp_acs_log);
  else if (context == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().access_log_list_.push_back(tmp_acs_log);
  else if (context == config::CONF_HTTP_LOCATION)
    this->config_.http_.server_list_.back().location_list_.back().access_log_list_.push_back(tmp_acs_log);

  return true;
}

bool config::Parser::parseLogLevel(config::ErrorLog &error_log, const std::string &level) {
  static std::map<std::string, LOG_LEVEL> log_level_map;
  log_level_map["debug"] = DEBUG;
  log_level_map["info"] = INFO;
  log_level_map["notice"] = NOTICE;
  log_level_map["warn"] = WARN;
  log_level_map["error"] = ERROR;
  log_level_map["crit"] = CRIT;
  log_level_map["alert"] = ALERT;
  log_level_map["emerg"] = EMERG;

  if (log_level_map.find(level) == log_level_map.end()) {
    printFormatedError("invalid log level", this->tokens_[ti_]);
    return false;
  }

  int mask = 0;
  switch (log_level_map[level]) {
    case DEBUG:
      mask |= DEBUG;  // fall through
    case INFO:
      mask |= INFO;  // fall through
    case NOTICE:
      mask |= NOTICE;  // fall through
    case WARN:
      mask |= WARN;  // fall through
    case ERROR:
      mask |= ERROR;  // fall through
    case CRIT:
      mask |= CRIT;  // fall through
    case ALERT:
      mask |= ALERT;  // fall through
    case EMERG:
      mask |= EMERG;  // fall through
  }
  error_log.setLevel(mask);
  return true;
}

bool config::Parser::parseErrorLog() {
  std::string path = this->tokens_[ti_].value_;
  config::CONTEXT context = this->current_context_.top();
  config::ErrorLog tmp_err_log;

  // 文字列が空の場合はobjectを追加しない
  if (path.empty()) {
    current_directive_.clear();
    return true;
  }
  tmp_err_log.setFile(path);

  if (this->tokens_[ti_ + 1].type_ != TK_SEMICOLON) {
    ++ti_;
    if (!parseLogLevel(tmp_err_log, this->tokens_[ti_].value_)) return false;
  }

  if (context == config::CONF_MAIN)
    this->config_.error_log_list_.push_back(tmp_err_log);
  else if (context == config::CONF_HTTP)
    this->config_.http_.error_log_list_.push_back(tmp_err_log);
  else if (context == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().error_log_list_.push_back(tmp_err_log);
  else if (context == config::CONF_HTTP_LOCATION)
    this->config_.http_.server_list_.back().location_list_.back().error_log_list_.push_back(tmp_err_log);

  return true;
}

#if defined(KQUEUE_AVAILABLE)
const config::OS currentOS = config::OS_BSD_BASED;
#elif defined(EPOLL_AVAILABLE)
const config::OS currentOS = config::OS_LINUX;
#else
const config::OS currentOS = config::OS_OTHER;
#endif

bool config::Parser::validEventType(const std::string &eventType) const {
  switch (currentOS) {
    case config::OS_BSD_BASED:
      return eventType == kSelect || eventType == kPoll || eventType == kKqueue;
    case config::OS_LINUX:
      return eventType == kSelect || eventType == kPoll || eventType == kEpoll;
    case config::OS_OTHER:
      return eventType == kSelect || eventType == kPoll;
  }
}

bool config::Parser::parseUse() {
  std::string token_value = this->tokens_[ti_].value_;

  if (!validEventType(token_value)) {
    printError(static_cast<std::string>("invalid event type \"" + token_value + "\""), tokens_[ti_]);
    return false;
  }

  std::map<std::string, config::CONNECTION_METHOD> method_map;
  config::CONNECTION_METHOD method;

  method_map[kSelect] = config::SELECT;
  method_map[kPoll] = config::POLL;
  method_map[kEpoll] = config::EPOLL;
  method_map[kKqueue] = config::KQUEUE;
  method = method_map.find(token_value)->second;

  this->config_.events_.use_.setConnectionMethod(method);

  return true;
}

bool config::Parser::parseWorkerConnections() {
  long value;
  std::string str = this->tokens_[ti_].value_;

  // 数値でなければエラー
  for (int i = 0; str[i] != '\0'; i++) {
    if (!std::isdigit(str[i])) {
      printFormatedError("invalid number", this->tokens_[ti_]);
      return false;
    }
  }

  std::istringstream iss(str);
  iss >> value;

  // 値が正の数かつLONG_MAX以内でなければエラー
  if (iss.fail() || iss.bad() || !iss.eof() || value < 0) {
    printFormatedError("invalid number", this->tokens_[ti_]);
    return false;
  }

  this->config_.events_.worker_connections_.setWorkerConnections(value);

  return true;
}

bool config::Parser::canConvertMinTime(long &value, const std::string &unit) {
  if (value == 0) return true;
  if (unit == "" || unit == "s") {
    if (config::Time::seconds > (config::Time::kMaxTimeInMilliseconds_ / value)) return false;
    value *= config::Time::seconds;
  } else if (unit == "m") {
    if (config::Time::minutes > (config::Time::kMaxTimeInMilliseconds_ / value)) return false;
    value *= config::Time::minutes;
  } else if (unit == "h") {
    if (config::Time::hours > (config::Time::kMaxTimeInMilliseconds_ / value)) return false;
    value *= config::Time::hours;
  } else if (unit == "d") {
    if (config::Time::days > (config::Time::kMaxTimeInMilliseconds_ / value)) return false;
    value *= config::Time::days;
  }
  return true;
}

bool config::Parser::canConvertMinSize(long &value, const std::string &unit) {
  if (value == 0) return true;
  if (unit == "k" || unit == "K") {
    if (config::Size::kilobytes > (config::Size::kMaxSizeInBytes_ / value)) return false;
    value *= config::Size::kilobytes;
  } else if (unit == "m" || unit == "M") {
    if (config::Size::megabytes > (config::Size::kMaxSizeInBytes_ / value)) return false;
    value *= config::Size::megabytes;
  }
  return true;
}

std::pair<long, std::string> config::Parser::parseValueWithUnit(const std::set<std::string> &units) const {
  long num;
  std::string unit;  // 単位
  std::istringstream iss(this->tokens_[ti_].value_.c_str());

  iss >> num;
  if (iss.fail() || iss.bad()) return std::make_pair(-1, "");

  // 残りをunitとして受け付ける
  if (iss.eof())
    unit = "";
  else {
    iss >> unit;
    if (iss.fail() || iss.bad() || !iss.eof()) return std::make_pair(-1, "");
  }

  // そのunitを受け付けているか
  if (!unit.empty() && units.find(unit) == units.end()) return std::make_pair(-1, "");

  if (num < 0) return std::make_pair(-1, "");

  return std::make_pair(num, unit);
}

long config::Parser::parseTime() {
  std::set<std::string> units;
  units.insert("ms");
  units.insert("m");
  units.insert("s");
  units.insert("h");
  units.insert("d");

  std::pair<long, std::string> time = parseValueWithUnit(units);

  // ms に変更できないとinvalid
  if (time.first == -1 || !canConvertMinTime(time.first, time.second)) return -1;

  return time.first;
}

long config::Parser::parseSize() {
  std::set<std::string> units;
  units.insert("k");
  units.insert("K");
  units.insert("m");
  units.insert("M");

  std::pair<long, std::string> size = parseValueWithUnit(units);

  // ms に変更できないとinvalid
  // k に変更できないとinvalid
  if (size.first == -1 || !canConvertMinSize(size.first, size.second)) return -1;

  return size.first;
}

bool config::Parser::parseSendTimeout() {
  long ret = parseTime();
  if (ret == -1) {
    printError("\"send_timeout\" directive invalid value", this->tokens_[ti_]);
    return false;
  }

  config::CONTEXT context = this->current_context_.top();

  if (context == config::CONF_HTTP)
    this->config_.http_.send_timeout_.setTime(ret);
  else if (context == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().send_timeout_.setTime(ret);
  else if (context == config::CONF_HTTP_LOCATION)
    this->config_.http_.server_list_.back().location_list_.back().send_timeout_.setTime(ret);

  return true;
}

bool config::Parser::parseKeepaliveTimeout() {
  long ret = parseTime();
  if (ret == -1) {
    printError("\"keepalive_timeout\" directive invalid value", this->tokens_[ti_]);
    return false;
  }

  const config::CONTEXT context = this->current_context_.top();
  switch (context) {
    case config::CONF_HTTP:
      this->config_.http_.keepalive_timeout_.setTime(ret);
      break;

    case config::CONF_HTTP_SERVER:
      this->config_.http_.server_list_.back().keepalive_timeout_.setTime(ret);
      break;

    case config::CONF_HTTP_LOCATION:
      this->config_.http_.server_list_.back().location_list_.back().keepalive_timeout_.setTime(ret);
      break;

    default:
      break;
  }

  return true;
}

bool config::Parser::parseReceiveTimeout() {
  long ret = parseTime();
  if (ret == -1) {
    printError("\"receive_timeout\" directive invalid value", this->tokens_[ti_]);
    return false;
  }

  const config::CONTEXT context = this->current_context_.top();
  switch (context) {
    case config::CONF_HTTP:
      this->config_.http_.receive_timeout_.setTime(ret);
      break;

    case config::CONF_HTTP_SERVER:
      this->config_.http_.server_list_.back().receive_timeout_.setTime(ret);
      break;

    case config::CONF_HTTP_LOCATION:
      this->config_.http_.server_list_.back().location_list_.back().receive_timeout_.setTime(ret);
      break;

    default:
      break;
  }

  return true;
}

bool config::Parser::parseRoot() {
  std::string path = this->tokens_[ti_].value_;
  config::CONTEXT context = this->current_context_.top();

  if (context == config::CONF_HTTP)
    this->config_.http_.root_.setPath(path);
  else if (context == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().root_.setPath(path);
  else if (context == config::CONF_HTTP_LOCATION) {
    std::set<std::string> &location_directives =
        this->config_.http_.server_list_.back().location_list_.back().directives_set_;
    if (location_directives.find(kAlias) != location_directives.end()) {
      printError("\"root\" directive is duplicate, \"alias\" directive was specified earlier in ",
                 this->tokens_[ti_]);
      return false;
    }
    this->config_.http_.server_list_.back().location_list_.back().root_.setPath(path);
  }

  return true;
}

bool config::Parser::parseClientMaxBodySize() {
  long ret = parseSize();
  if (ret == -1) {
    printError("\"client_max_body_size\" directive invalid value", this->tokens_[ti_]);
    return false;
  }

  this->config_.http_.client_max_body_size_.setSize(ret);

  return true;
}

bool config::Parser::parseIndex() {
  std::string file;
  config::CONTEXT context = this->current_context_.top();
  config::Index tmp_index;

  while (this->tokens_[ti_].type_ != config::TK_SEMICOLON) {
    file = this->tokens_[ti_].value_;

    // 空文字列があればエラー
    if (file.empty()) {
      printError(static_cast<std::string>("index \"" + file + "\" in \"index\" directive is invalid"),
                 this->tokens_[ti_]);
      return false;
    }

    tmp_index.setFile(file);

    if (context == config::CONF_HTTP)
      this->config_.http_.index_list_.push_back(tmp_index);
    else if (context == config::CONF_HTTP_SERVER)
      this->config_.http_.server_list_.back().index_list_.push_back(tmp_index);
    else if (context == config::CONF_HTTP_LOCATION)
      this->config_.http_.server_list_.back().location_list_.back().index_list_.push_back(tmp_index);

    ++ti_;
  }

  --ti_;  // parseDirectiveで2進めるので1戻す
  return true;
}

bool config::Parser::parseAutoindex() {
  std::string tmp_switch = this->tokens_[ti_].value_;

  if (tmp_switch != "on" && tmp_switch != "off") {
    printError(static_cast<std::string>("invalid value \"" + tmp_switch +
                                        "\" in \"autoindex\" directive, it must be \"on\" or \"off\""),
               this->tokens_[ti_]);
    return false;
  }

  config::CONTEXT context = this->current_context_.top();

  if (tmp_switch == "on") {
    if (context == config::CONF_HTTP)
      this->config_.http_.autoindex_.setIsAutoindexOn(true);
    else if (context == config::CONF_HTTP_SERVER)
      this->config_.http_.server_list_.back().autoindex_.setIsAutoindexOn(true);
    else if (context == config::CONF_HTTP_LOCATION)
      this->config_.http_.server_list_.back().location_list_.back().autoindex_.setIsAutoindexOn(true);
  }

  return true;
}

unsigned int config::Parser::retCodeIfValid() {
  std::istringstream iss(this->tokens_[ti_].value_.c_str());
  long code;

  iss >> code;
  if (iss.fail() || iss.bad() || !iss.eof()) {
    printFormatedError("invalid value", this->tokens_[ti_]);
    return 0;
  }

  if (!(300 <= code && code <= 599)) {
    printError(static_cast<std::string>("value \"" + utils::toStr(code) + "\" must be between 300 and 599"),
               this->tokens_[ti_]);
    return 0;
  }

  return static_cast<unsigned int>(code);
}

long config::Parser::retErrorPageOptNumIfValid() {
  std::istringstream iss(this->tokens_[ti_].value_.substr(1));
  long tmp_code;

  // LONG_MAX/MIN を超えたり、数値ではなければエラー
  iss >> tmp_code;
  if (iss.fail() || iss.bad() || !iss.eof() || tmp_code < 0) {
    printFormatedError("invalid value", this->tokens_[ti_]);
    return -1;
  }
  return tmp_code;
}

bool config::Parser::parseErrorPage() {
  config::ErrorPage tmp_err_pg;
  unsigned int code;
  unsigned int tmp_ti = ti_;

  // uriまではcodeとしてみていく
  while (this->tokens_[ti_ + 1].type_ != config::TK_SEMICOLON) {
    // 最後から二番目の引数が=responseオプションの場合
    if (ti_ != tmp_ti && this->tokens_[ti_ + 2].type_ == config::TK_SEMICOLON &&
        tokens_[ti_].value_[0] == '=') {
      long response = retErrorPageOptNumIfValid();
      if (response == -1) return false;
      tmp_err_pg.setResponse(response);
      ti_++;
      break;
    }

    code = retCodeIfValid();
    if (!code) return false;

    tmp_err_pg.addCode(code);

    ++ti_;
  }

  tmp_err_pg.setUri(this->tokens_[ti_].value_);

  if (this->current_context_.top() == config::CONF_HTTP)
    this->config_.http_.error_page_list_.push_back(tmp_err_pg);
  else if (this->current_context_.top() == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().error_page_list_.push_back(tmp_err_pg);
  else if (this->current_context_.top() == config::CONF_HTTP_LOCATION)
    this->config_.http_.server_list_.back().location_list_.back().error_page_list_.push_back(tmp_err_pg);

  return true;
}

bool config::Parser::isIPv4(const std::string &ipv4) const {
  if (ipv4.empty()) {
    return false;
  }

  // IPv4アドレスとサブネットマスクを分割
  size_t mask_pos = ipv4.find('/');
  std::string address_part = ipv4.substr(0, mask_pos);
  std::string mask_part = (mask_pos != std::string::npos) ? ipv4.substr(mask_pos + 1) : "";

  // 文字列がIPv4の基本的な構造に従っているかを確認
  std::istringstream iss(address_part);
  std::string field;
  std::vector<std::string> fields;

  while (std::getline(iss, field, '.')) {
    // 各フィールドが数字であることを確認
    for (int i = 0; field[i] != '\0'; i++) {
      if (!std::isdigit(field[i])) {
        return false;
      }
    }

    fields.push_back(field);
  }

  // フィールドの数が正しいかを確認
  if (fields.size() != 4) {
    return false;
  }

  // 各フィールドが0から255までの値を持っていることを確認
  for (int i = 0; i < 4; i++) {
    field = fields[i];
    unsigned int value;
    iss.clear();
    iss.str(field);
    iss >> value;
    if (255 < value) {
      return false;
    }
  }

  // subnetmaskの値が正しいか確認
  if (!(mask_part.empty() || isNumInRange(mask_part, 0, 32))) return false;

  // 全ての条件を満たす場合、IPv4アドレスと見なす
  return true;
}

bool config::Parser::isIPv6(const std::string &ipv6) const {
  if (ipv6.empty()) {
    return false;
  }

  // IPv6アドレスとサブネットマスクを分割
  size_t mask_pos = ipv6.find('/');
  std::string address_part = (mask_pos != std::string::npos) ? ipv6.substr(0, mask_pos) : ipv6;
  std::string mask_part = (mask_pos != std::string::npos) ? ipv6.substr(mask_pos + 1) : "";

  // 文字列がIPv6の基本的な構造に従っているかを確認
  std::istringstream iss(address_part);
  std::string field;
  std::vector<std::string> fields;

  while (std::getline(iss, field, ':')) {
    // 各フィールドが16進数であることを確認
    for (int i = 0; field[i] != '\0'; i++) {
      if (!std::isxdigit(field[i])) {
        return false;
      }
    }

    fields.push_back(field);
  }

  // フィールドの数が正しいかを確認
  if (fields.size() < 2 || fields.size() > 8) {
    return false;
  }

  // 各フィールドが0からFFFFまでの値を持っていることを確認
  for (size_t i = 0; i < fields.size(); i++) {
    field = fields[i];
    if (field.empty())  // "::"によるから文字は許容する
      continue;
    unsigned int value;
    iss.clear();
    iss.str(field);
    iss >> std::hex >> value;
    if (value > 0xFFFF) {
      return false;
    }
  }

  // subnetmaskの値が正しいか確認
  if (!mask_part.empty() && !isNumInRange(mask_part, 0, 128)) return false;

  // 全ての条件を満たす場合、IPv6アドレスと見なす
  return true;
}

/**
 * @brief IPv6とIPv4がmixされたアドレスかを判定
 *
 * @param mixed_ip
 * @return true
 * @return false
 */
bool config::Parser::isMixedIPAddress(const std::string &mixed_ip) const {
  if (mixed_ip.empty()) {
    return false;
  }
  size_t ipv6_pos = mixed_ip.rfind(":");
  if (ipv6_pos == std::string::npos) return false;
  ++ipv6_pos;
  size_t subnet_pos = mixed_ip.rfind("/");
  const std::string mask_part = subnet_pos != std::string::npos ? mixed_ip.substr(subnet_pos + 1) : "";
  const std::string ipv6 = mixed_ip.substr(0, ipv6_pos);
  const std::string ipv4 = subnet_pos == std::string::npos ? mixed_ip.substr(ipv6_pos)
                                                           : mixed_ip.substr(ipv6_pos, subnet_pos - ipv6_pos);
  if (!mask_part.empty() && !isNumInRange(mask_part, 0, 128)) return false;

  return isIPv4(ipv4) && isIPv6(ipv6);
}

bool config::Parser::isNumInRange(const std::string &num, long min, long max) const {
  if (!utils::isNumeric(num)) return false;

  std::istringstream converter(num);
  long value;
  if (!(converter >> value)) return false;
  if (value < min || value > max) return false;
  return true;
}

/**
 * @brief parse allow or deny directive, same syntax
 *
 * @return true
 * @return false
 */
bool config::Parser::parseAllowDeny() {
  const config::ACCESS_DIRECTIVE directive_type = current_directive_ == kAllow ? config::ALLOW : config::DENY;
  const std::string address = this->tokens_[ti_].value_;

  // ipv6にも対応するならば、!isIPv6()と!isMixedIPAddress()も条件に追加してください。
  if (!(address == "all" || isIPv4(address))) {
    printError(std::string("invalid parameter \"" + address + "\""), this->tokens_[ti_]);
    return false;
  }

  config::AllowDeny tmp;
  tmp.setAccessDirective(directive_type);
  tmp.setAddress(address);
  const std::string directive_name = directive_type == config::ALLOW ? kAllow : kDeny;

  switch (this->current_context_.top()) {
    case config::CONF_HTTP:
      this->config_.http_.allow_deny_list_.push_back(tmp);
      break;

    case config::CONF_HTTP_SERVER:
      this->config_.http_.server_list_.back().allow_deny_list_.push_back(tmp);
      break;

    case config::CONF_HTTP_LOCATION:
      this->config_.http_.server_list_.back().location_list_.back().allow_deny_list_.push_back(tmp);
      break;

    case config::CONF_HTTP_LIMIT_EXCEPT:
      this->config_.http_.server_list_.back().location_list_.back().limit_except_.allow_deny_list_.push_back(
          tmp);
      break;

    default:
      break;
  }

  return true;
}

bool config::Parser::parseListen() {
  std::string ori_val = this->tokens_[ti_].value_;
  config::Listen tmp_listen;
  std::istringstream iss;
  long port = 80;
  std::string segment;
  std::vector<std::string> segments;

  // 1. もし空文字列ならエラー
  if (ori_val.empty()) {
    printError(static_cast<std::string>("host not found in \"" + utils::toStr(ori_val) +
                                        "\" of the \"listen\" directive"),
               this->tokens_[ti_]);
    return false;
  }

  // 2. ip addressとportに分ける
  // ':'の領域が空なら空文字列を入れる。
  // ':'だけの場合はエラー
  if (ori_val == ":") {
    printError(static_cast<std::string>("invalid port in \"" + utils::toStr(ori_val) +
                                        "\" of the \"listen\" directive"),
               this->tokens_[ti_]);
    return false;
  }

  iss.str(ori_val.c_str());
  while (getline(iss, segment, ':')) {
    segments.push_back(segment);
  }

  if (ori_val[ori_val.size() - 1] == ':') {
    segments.push_back("");
  }

  // 3. 2つ以上に分かれてしまっていたらエラー
  if (segments.size() > 2) {
    printError(static_cast<std::string>("invalid parameter \"" + utils::toStr(ori_val) + "\""),
               this->tokens_[ti_]);
    return false;
  }

  // 4. ip address とportの両方がある場合
  if (segments.size() == 2) {
    // ip addressがあれば値を確認する。
    if (segments[0].empty()) {
      printError(static_cast<std::string>("no host in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    }
    // ipv6にも対応するならば、!isIPv6()も条件に加えてください。
    if (!isIPv4(segments[0])) {
      printError(static_cast<std::string>("host not found in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    }
    if (segments[0].find('/') != std::string::npos) {
      printError(static_cast<std::string>("invalid host in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    }
    tmp_listen.setAddress(segments[0]);

    // port番号があれば値を確認しする。
    if (segments[1].empty()) {
      printError(static_cast<std::string>("invalid port in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    }

    iss.clear();
    iss.str(segments[1].c_str());
    iss >> port;
    if (iss.fail() || iss.bad() || !iss.eof()) {
      printError(static_cast<std::string>("host not found in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    }

    if (port < 0 || 65535 < port) {
      printError(static_cast<std::string>("invalid port in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    }
    tmp_listen.setPort(port);
  } else {
    // 5. ip addressかportのどちらかしかない場合
    iss.clear();
    iss.str(segments[0].c_str());
    long tmp_port;
    iss >> tmp_port;

    // ip addressかportであればセット
    // ipv6も対応するならば、isIPv6())も条件に加えてください。
    if (isIPv4(segments[0]))
      tmp_listen.setAddress(segments[0]);
    else if (iss.fail() || iss.bad() || !iss.eof()) {
      printError(static_cast<std::string>("host not found in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    } else if (tmp_port < 0 || 65535 < tmp_port) {
      printError(static_cast<std::string>("invalid port in \"" + utils::toStr(ori_val) +
                                          "\" of the \"listen\" directive"),
                 this->tokens_[ti_]);
      return false;
    } else  // 有効なport
      tmp_listen.setPort(tmp_port);
  }

  // 6. defalt_serverがあるばあい
  if (this->tokens_[ti_ + 1].type_ != config::TK_SEMICOLON) {
    ti_++;
    // tmp_listen.setPort(port);

    if (this->tokens_[ti_].value_ != "default_server") {
      printFormatedError("invalid parameter", this->tokens_[ti_]);
      return false;
    }
    if (isDuplicateDefaultServer(tmp_listen)) {
      printError(static_cast<std::string>("a duplicate default server for " +
                                          utils::toStr(this->tokens_[ti_ - 1].value_)),
                 this->tokens_[ti_]);
      return false;
    }
    tmp_listen.setIsDefaultServer(true);
  }

  this->config_.http_.server_list_.back().listen_list_.push_back(tmp_listen);

  return true;
}

bool config::Parser::isDuplicateDefaultServer(const config::Listen &this_listen) {
  std::vector<config::Server> &server_list = this->config_.http_.server_list_;

  for (size_t si = 0; si < server_list.size(); si++) {
    for (size_t li = 0; li < server_list[si].listen_list_.size(); li++) {
      config::Listen &another_listen = server_list[si].listen_list_[li];
      if (another_listen.getIsDefaultServer() && another_listen.getAddress() == this_listen.getAddress() &&
          another_listen.getport() == this_listen.getport()) {
        return true;
      }
    }
  }

  return false;
}

bool config::Parser::parseServerName() {
  // 最初のserver_nameディレクティブであれば、デフォルト値を削除する
  if (this->config_.http_.server_list_.back().directives_set_.find(kServerName) ==
      this->config_.http_.server_list_.back().directives_set_.end())
    this->config_.http_.server_list_.back().server_name_.eraseDefaultName();

  while (this->tokens_[ti_].type_ != config::TK_SEMICOLON) {
    this->config_.http_.server_list_.back().server_name_.addName(this->tokens_[ti_].value_);

    ++ti_;
  }

  --ti_;
  return true;
}

bool config::Parser::parseTryFiles() {
  config::CONTEXT context = this->current_context_.top();
  std::string file;

  // 最後から2つ目までのトークンはfileとして追加
  while (this->tokens_[ti_ + 1].type_ != config::TK_SEMICOLON) {
    file = this->tokens_[ti_].value_;

    if (context == config::CONF_HTTP_SERVER)
      this->config_.http_.server_list_.back().try_files_.addFile(file);
    else if (context == config::CONF_HTTP_LOCATION)
      this->config_.http_.server_list_.back().location_list_.back().try_files_.addFile(file);

    ++ti_;
  }

  // codeかuriか判定
  std::string uri;
  int code;
  std::istringstream iss;

  if (this->tokens_[ti_].value_[0] == '=') {
    // code
    iss.str(this->tokens_[ti_].value_.substr(1));

    // code が正しいか判定
    iss >> code;
    if (iss.fail() || iss.bad() || !iss.eof() || code < 0 || 999 < code) {
      printFormatedError("invalid code", this->tokens_[ti_]);
      return false;
    }

    // setCode
    if (context == config::CONF_HTTP_SERVER)
      this->config_.http_.server_list_.back().try_files_.setCode(code);
    else
      this->config_.http_.server_list_.back().location_list_.back().try_files_.setCode(code);
  } else {
    // uri
    uri = this->tokens_[ti_].value_;

    // setUri
    if (context == config::CONF_HTTP_SERVER)
      this->config_.http_.server_list_.back().try_files_.setUri(uri);
    else if (context == config::CONF_HTTP_LOCATION)
      this->config_.http_.server_list_.back().location_list_.back().try_files_.setUri(uri);
  }

  return true;
}

bool config::Parser::parseAlias() {
  std::string path = this->tokens_[ti_].value_;

  std::set<std::string> &location_directives =
      this->config_.http_.server_list_.back().location_list_.back().directives_set_;
  if (location_directives.find(kRoot) != location_directives.end()) {
    printError("\"alias\" directive is duplicate, \"root\" directive was specified earlier",
               this->tokens_[ti_]);
    return false;
  }

  this->config_.http_.server_list_.back().location_list_.back().alias_.setPath(path);

  return true;
}

bool config::Parser::parseReturn() {
  long code;
  config::Return tmp_return;
  const std::string http = "http://";
  const std::string https = "https://";

  if (utils::isNumeric(this->tokens_[ti_].value_)) {
    if (!isNumInRange(this->tokens_[ti_].value_, 0, 999)) {
      printFormatedError("invalid return code", this->tokens_[ti_]);
      return false;
    }
    std::istringstream iss(this->tokens_[ti_].value_);
    iss >> code;
    tmp_return.setCode(code);
    ++ti_;
  }

  // urlの場合のみ文字列をチェックする
  code = tmp_return.getCode();
  if (this->tokens_[ti_].type_ != config::TK_SEMICOLON && code == config::Return::kCodeUnset_) {
    const std::string url = this->tokens_[ti_].value_;
    if (url.substr(0, http.length()) != http && url.substr(0, https.length()) != https) {
      printFormatedError("invalid return code", this->tokens_[ti_]);
      return false;
    }
    tmp_return.setUrl(this->tokens_[ti_].value_);
    ++ti_;
  } else if (this->tokens_[ti_].type_ != config::TK_SEMICOLON) {
    tmp_return.setUrl(this->tokens_[ti_].value_);  // textをset
    ++ti_;
  }

  this->config_.http_.server_list_.back().location_list_.back().return_list_.push_back(tmp_return);

  --ti_;  // parseDirectiveで2進めるので1戻す
  return true;
}

bool config::Parser::parseUserid() {
  std::string tmp_switch = this->tokens_[ti_].value_;

  // もし、on/offではなかったらエラー
  if (tmp_switch != "on" && tmp_switch != "off") {
    printError(static_cast<std::string>("invalid value" + tmp_switch +
                                        "\" in \"userid\" directive, it must be \"on\" or \"off\""),
               this->tokens_[ti_]);
    return false;
  }

  config::CONTEXT context = this->current_context_.top();

  // もし、onであれば、trueにする
  if (tmp_switch == "on") {
    if (context == config::CONF_HTTP)
      this->config_.http_.userid_.setIsUseridOn(true);
    else if (context == config::CONF_HTTP_SERVER)
      this->config_.http_.server_list_.back().userid_.setIsUseridOn(true);
    else if (context == config::CONF_HTTP_LOCATION)
      this->config_.http_.server_list_.back().location_list_.back().userid_.setIsUseridOn(true);
  }

  return true;
}

bool config::Parser::parseUseridDomain() {
  std::string name = this->tokens_[ti_].value_;
  config::CONTEXT context = this->current_context_.top();

  if (context == config::CONF_HTTP)
    this->config_.http_.userid_domain_.setName(name);
  else if (context == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().userid_domain_.setName(name);
  else if (context == config::CONF_HTTP_LOCATION)
    this->config_.http_.server_list_.back().location_list_.back().userid_domain_.setName(name);

  return true;
}

bool config::Parser::parseUseridExpires() {
  std::string tmp_switch = this->tokens_[ti_].value_;
  config::CONTEXT context = this->current_context_.top();

  // off であれば、なにもしない
  if (tmp_switch == "off") {
    return true;
  }

  long time = parseTime();
  if (time == -1) {
    printError("\"userid_expires\" directive invalid value", this->tokens_[ti_]);
    return false;
  }

  if (context == config::CONF_HTTP) {
    this->config_.http_.userid_expires_.setTime(time);
    this->config_.http_.userid_expires_.setIsUseridExpiresOn(true);
  } else if (context == config::CONF_HTTP_SERVER) {
    this->config_.http_.server_list_.back().userid_expires_.setTime(time);
    this->config_.http_.server_list_.back().userid_expires_.setIsUseridExpiresOn(true);
  } else if (context == config::CONF_HTTP_LOCATION) {
    this->config_.http_.server_list_.back().location_list_.back().userid_expires_.setTime(time);
    this->config_.http_.server_list_.back().location_list_.back().userid_expires_.setIsUseridExpiresOn(true);
  }

  return true;
}

bool config::Parser::parseUseridPath() {
  std::string path = this->tokens_[ti_].value_;
  config::CONTEXT context = this->current_context_.top();

  // もし値が空文字列でなければセットする
  if (!path.empty()) {
    if (context == config::CONF_HTTP) {
      this->config_.http_.userid_path_.setPath(path);
    } else if (context == config::CONF_HTTP_SERVER) {
      this->config_.http_.server_list_.back().userid_path_.setPath(path);
    } else if (context == config::CONF_HTTP_LOCATION) {
      this->config_.http_.server_list_.back().location_list_.back().userid_path_.setPath(path);
    }
  }

  return true;
}

bool config::Parser::parseUseridService() {
  long user_id;
  std::istringstream iss(this->tokens_[ti_].value_.c_str());

  iss >> user_id;
  if (iss.fail() || iss.bad() || !iss.eof()) {
    printError("\"userid_service\" directive invalid value", this->tokens_[ti_]);
    return false;
  }

  config::CONTEXT context = this->current_context_.top();

  if (context == config::CONF_HTTP)
    this->config_.http_.userid_service_.setUseridService(user_id);
  else if (context == config::CONF_HTTP_SERVER)
    this->config_.http_.server_list_.back().userid_service_.setUseridService(user_id);
  else if (context == config::CONF_HTTP_LOCATION)
    this->config_.http_.server_list_.back().location_list_.back().userid_service_.setUseridService(user_id);

  return true;
}

const config::Main &config::Parser::getConfig() const { return this->config_; }

void config::Parser::updateDirectivesSet(const std::string &directive) {
  switch (current_context_.top()) {
    case CONF_MAIN:
      config_.directives_set_.insert(directive);
      break;
    case CONF_EVENTS:
      config_.events_.directives_set_.insert(directive);
      break;
    case CONF_HTTP:
      config_.http_.directives_set_.insert(directive);
      break;
    case CONF_HTTP_SERVER:
      config_.http_.server_list_.back().directives_set_.insert(directive);
      break;
    case CONF_HTTP_LOCATION:
      config_.http_.server_list_.back().location_list_.back().directives_set_.insert(directive);
      break;
    case CONF_HTTP_LIMIT_EXCEPT:
      config_.http_.server_list_.back().location_list_.back().limit_except_.directives_set_.insert(directive);
      break;
  }
}
