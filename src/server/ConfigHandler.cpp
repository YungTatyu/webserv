#include "ConfigHandler.hpp"

#include <sys/socket.h>

#include <cerrno>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <ostream>

#include "NetworkIOHandler.hpp"
#include "WebServer.hpp"
#include "error.hpp"
#include "utils.hpp"

static const std::string kAccessLog = "access_log";
static const std::string kErrorLog = "error_log";
static const std::string kKeepaliveTimeout = "keepalive_timeout";
static const std::string kReceiveTimeout = "receive_timeout";
static const std::string kSendTimeout = "send_timeout";
static const std::string kUseridExpires = "userid_expires";
static const std::string kDeny = "deny";
static const std::string kAlias = "alias";
static const std::string kAutoindex = "autoindex";
static const std::string kRoot = "root";
static const std::string kErrorPage = "error_page";
static const std::string kLimitExcept = "limit_except";

ConfigHandler::ConfigHandler() : config_(NULL) {}

ConfigHandler::~ConfigHandler() {}

/** Configにあってほしい機能
 * デフォルトサーバがどれか
 *  ipとmask分けて保存
 */

/* ConfigHandlerクラスの実装 */
void ConfigHandler::loadConfiguration(const config::Main* config) {
  /* ConfファイルをパースしてデータをServConfigクラスにセットする */
  this->config_ = config;
}

bool ConfigHandler::addressInLimit(const std::string& ip_addr_str, uint32_t cli_addr) const {
  if (ip_addr_str == "all") return true;
  std::istringstream iss(ip_addr_str);
  std::string ip;
  std::string mask;

  std::getline(iss, ip, '/');
  std::getline(iss, mask);

  uint32_t conf_addr = utils::strToIPAddress(ip);
  uint32_t mask_val = 0xFFFFFFFF;

  // サブネットマスクが指定されている場合
  if (!mask.empty()) {
    int prefix_length;
    iss.clear();
    iss.str(mask);
    iss >> prefix_length;
    mask_val <<= (32 - prefix_length);
  }

  return (conf_addr & mask_val) == (cli_addr & mask_val);
}

bool ConfigHandler::limitLoop(const std::vector<config::AllowDeny>& allow_deny_list,
                              uint32_t cli_addr) const {
  // 上から順に制限適用する
  // 制限されているアドレスであれば、false
  // エラーページどのタイミングで返すか？
  for (size_t i = 0; i < allow_deny_list.size(); i++) {
    if (addressInLimit(allow_deny_list[i].getAddress(), cli_addr)) {
      switch (allow_deny_list[i].getAccessDirective()) {
        case config::ALLOW:
          return true;
          break;
        case config::DENY:
          return false;
          break;
        default:
          return false;
          break;
      }
    }
  }

  return true;
}

int ConfigHandler::allowRequest(const config::Server& server, const config::Location* location,
                                const HttpRequest& request, struct sockaddr_in client_addr) const {
  // ------ access の制限 ------
  // configからアドレス制限ディレクトリのあるcontext探す
  if (location && utils::hasDirective(*location, kDeny)) {
    if (!limitLoop(location->allow_deny_list_, client_addr.sin_addr.s_addr)) return ACCESS_DENY;
  } else if (utils::hasDirective(server, kDeny)) {
    if (!limitLoop(server.allow_deny_list_, client_addr.sin_addr.s_addr)) return ACCESS_DENY;
  } else if (utils::hasDirective(this->config_->http_, kDeny)) {
    if (!limitLoop(this->config_->http_.allow_deny_list_, client_addr.sin_addr.s_addr)) return ACCESS_DENY;
  }

  // ------ method の制限 ------
  // location内にlimit_except contextあるか？
  if (location && utils::hasDirective(*location, kLimitExcept)) {
    // 制限されたメソッドでなければ、スルー
    // HttpRequestでLIMIT_EXCEPTのenum使ってほしい
    if (location->limit_except_.excepted_methods_.find(request.method_) ==
        location->limit_except_.excepted_methods_.end()) {
      if (!limitLoop(location->limit_except_.allow_deny_list_, client_addr.sin_addr.s_addr))
        return METHOD_DENY;
    }
  }

  // 問題なければtrue
  return ACCESS_ALLOW;
}

// 最終的なlocationで記録
void ConfigHandler::writeAccessLog(const struct TiedServer& tied_servers, const std::string& server_name,
                                   const std::string& uri, const std::string& msg) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);
  writeAccessLog(server, location, msg);
}

void ConfigHandler::writeAccessLog(const config::Server& server, const config::Location* location,
                                   const std::string& msg) const {
  // access_logがコンテキストで指定されていれば出力する
  if (location && utils::hasDirective(*location, kAccessLog)) {
    for (size_t i = 0; i < location->access_log_list_.size(); i++) {
      if (utils::writeChunks(location->access_log_list_[i].getFd(), msg) == -1)
        WebServer::writeErrorlog(error::strSysCallError("write"), config::ERROR);
    }
  }
  if (utils::hasDirective(server, kAccessLog)) {
    for (size_t i = 0; i < server.access_log_list_.size(); i++) {
      if (utils::writeChunks(server.access_log_list_[i].getFd(), msg) == -1)
        WebServer::writeErrorlog(error::strSysCallError("write"), config::ERROR);
    }
  }
  // directives_setになくてもデフォルトファイルがあるはずなので出力する
  for (size_t i = 0; i < this->config_->http_.access_log_list_.size(); i++) {
    if (utils::writeChunks(this->config_->http_.access_log_list_[i].getFd(), msg) == -1)
      WebServer::writeErrorlog(error::strSysCallError("write"), config::ERROR);
  }
}

void ConfigHandler::writeAccessLog(const std::string& msg) const {
  for (size_t i = 0; i < this->config_->http_.access_log_list_.size(); i++) {
    if (utils::writeChunks(this->config_->http_.access_log_list_[i].getFd(), msg) == -1)
      WebServer::writeErrorlog(error::strSysCallError("write"), config::ERROR);
  }
}

void ConfigHandler::writeErrorLog(const std::string& msg, config::LOG_LEVEL level) const {
  std::string formated_msg = formatErrorLogMsg(msg, level);
  // http conterxtにerror_logディレクティブがあれば出力
  if (utils::hasDirective(this->config_->http_, kErrorLog)) {
    for (size_t i = 0; i < this->config_->http_.error_log_list_.size(); i++) {
      if (!(this->config_->http_.error_log_list_[i].getLevel() & level)) continue;
      if (utils::writeChunks(this->config_->http_.error_log_list_[i].getFd(), formated_msg) == -1)
        error::printError(error::strSysCallError("write"), config::WARN);
    }
  }
  // main contextにerror_logディレクティブがなくてもデフォルトに出力する
  // fdがopenできていなければサーバーは動いていないので特に確認しなくていい。
  for (size_t i = 0; i < this->config_->error_log_list_.size(); i++) {
    if (!(this->config_->error_log_list_[i].getLevel() & level)) continue;
    if (utils::writeChunks(this->config_->error_log_list_[i].getFd(), formated_msg) == -1)
      error::printError(error::strSysCallError("write"), config::WARN);
  }
}

void ConfigHandler::writeErrorLog(const struct TiedServer& tied_servers, const std::string& server_name,
                                  const std::string& uri, const std::string& msg,
                                  config::LOG_LEVEL level) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);
  writeErrorLog(server, location, msg, level);
}

void ConfigHandler::writeErrorLog(const config::Server& server, const config::Location* location,
                                  const std::string& msg, config::LOG_LEVEL level) const {
  std::string formated_msg = formatErrorLogMsg(msg, level);
  if (location && utils::hasDirective(*location, kErrorLog)) {
    for (size_t i = 0; i < location->error_log_list_.size(); i++) {
      if (!(location->error_log_list_[i].getLevel() & level)) continue;
      if (utils::writeChunks(location->error_log_list_[i].getFd(), formated_msg) == -1)
        error::printError(error::strSysCallError("write"), config::WARN);
    }
  }
  if (utils::hasDirective(server, kErrorLog)) {
    for (size_t i = 0; i < server.error_log_list_.size(); i++) {
      if (!(server.error_log_list_[i].getLevel() & level)) continue;
      if (utils::writeChunks(server.error_log_list_[i].getFd(), formated_msg) == -1)
        error::printError(error::strSysCallError("write"), config::WARN);
    }
  }
  if (utils::hasDirective(this->config_->http_, kErrorLog)) {
    for (size_t i = 0; i < this->config_->http_.error_log_list_.size(); i++) {
      if (!(this->config_->http_.error_log_list_[i].getLevel() & level)) continue;
      if (utils::writeChunks(this->config_->http_.error_log_list_[i].getFd(), formated_msg) == -1)
        error::printError(error::strSysCallError("write"), config::WARN);
    }
  }
  // ほかのコンテキストで設定されていなければ、デフォルトファイルに出力する。
  for (size_t i = 0; i < this->config_->error_log_list_.size(); i++) {
    if (!(this->config_->error_log_list_[i].getLevel() & level)) continue;
    if (utils::writeChunks(this->config_->error_log_list_[i].getFd(), formated_msg) == -1)
      error::printError(error::strSysCallError("write"), config::WARN);
  }
}

const config::Time& ConfigHandler::searchKeepaliveTimeout(const struct TiedServer& tied_servers,
                                                          const std::string& server_name,
                                                          const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && utils::hasDirective(*location, kKeepaliveTimeout)) {
    return location->keepalive_timeout_.getTime();
  } else if (utils::hasDirective(server, kKeepaliveTimeout)) {
    return server.keepalive_timeout_.getTime();
  } else  // http兼default
  {
    return this->config_->http_.keepalive_timeout_.getTime();
  }
}

const config::Time& ConfigHandler::searchReceiveTimeout(const struct TiedServer& tied_servers,
                                                        const std::string& server_name,
                                                        const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && utils::hasDirective(*location, kReceiveTimeout)) {
    return location->receive_timeout_.getTime();
  } else if (utils::hasDirective(server, kReceiveTimeout)) {
    return server.receive_timeout_.getTime();
  }
  // http兼default
  return this->config_->http_.receive_timeout_.getTime();
}

const config::Time& ConfigHandler::searchSendTimeout(const TiedServer& tied_servers,
                                                     const std::string& server_name,
                                                     const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && utils::hasDirective(*location, kSendTimeout)) {
    return location->send_timeout_.getTime();
  } else if (utils::hasDirective(server, kSendTimeout)) {
    return server.send_timeout_.getTime();
  } else  // http兼default
  {
    return this->config_->http_.send_timeout_.getTime();
  }
}

const config::Time& ConfigHandler::searchUseridExpires(const struct TiedServer& tied_servers,
                                                       const std::string& server_name,
                                                       const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && utils::hasDirective(*location, kUseridExpires)) {
    return location->userid_expires_.getTime();
  } else if (utils::hasDirective(server, kUseridExpires)) {
    return server.userid_expires_.getTime();
  } else  // http兼default
  {
    return this->config_->http_.userid_expires_.getTime();
  }
}

const config::Server& ConfigHandler::searchServerConfig(const struct TiedServer& tied_servers,
                                                        const std::string& server_name) const {
  const config::Server* default_server = tied_servers.servers_[0];

  for (size_t si = 0; si < tied_servers.servers_.size(); si++) {
    if (server_name != "" && tied_servers.servers_[si]->server_name_.getName().find(server_name) !=
                                 tied_servers.servers_[si]->server_name_.getName().end())
      return *tied_servers.servers_[si];
    for (size_t li = 0; li < tied_servers.servers_[si]->listen_list_.size(); li++) {
      const config::Listen& tmp_listen = tied_servers.servers_[si]->listen_list_[li];
      // default_serverであれば更新
      if (tmp_listen.getIsDefaultServer() && tied_servers.port_ == tmp_listen.getport() &&
          tied_servers.addr_ == tmp_listen.getAddress()) {
        default_server = tied_servers.servers_[si];
      }
    }
  }

  // server_nameが一致するものがなければデフォルトサーバーを返す
  return *default_server;
}

/*
 * uriが最長一致しているlocationを返す。
 */
const config::Location* ConfigHandler::searchLongestMatchLocationConfig(const config::Server& server_config,
                                                                        const std::string& uri) const {
  const config::Location* longest_match = NULL;
  size_t max_len = 0;

  for (size_t i = 0; i < server_config.location_list_.size(); i++) {
    std::string config_uri = server_config.location_list_[i].uri_;
    if (uri.find(config_uri) == 0 && max_len < config_uri.size()) {
      max_len = std::max(max_len, config_uri.size());
      longest_match = &server_config.location_list_[i];
    }
  }
  return longest_match;
}

const config::ErrorPage* ConfigHandler::searchErrorPage(const config::Server& server,
                                                        const config::Location* location,
                                                        unsigned int code) const {
  if (location && utils::hasDirective(*location, kErrorPage)) {
    const std::vector<config::ErrorPage>& ep_list = location->error_page_list_;
    for (size_t i = 0; i < ep_list.size(); i++) {
      if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end()) {
        return &ep_list[i];
      }
    }
  } else if (utils::hasDirective(server, kErrorPage)) {
    const std::vector<config::ErrorPage>& ep_list = server.error_page_list_;
    for (size_t i = 0; i < ep_list.size(); i++) {
      if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end()) {
        return &ep_list[i];
      }
    }
  } else if (utils::hasDirective(this->config_->http_, kErrorPage)) {
    const std::vector<config::ErrorPage>& ep_list = this->config_->http_.error_page_list_;
    for (size_t i = 0; i < ep_list.size(); i++) {
      if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end()) {
        return &ep_list[i];
      }
    }
  }

  return NULL;
}

struct TiedServer ConfigHandler::createTiedServer(const std::string& addr, unsigned int port) const {
  struct TiedServer tied_server(addr, port);

  for (size_t i = 0; i < config_->http_.server_list_.size(); i++) {
    for (size_t j = 0; j < config_->http_.server_list_[i].listen_list_.size(); j++) {
      if (config_->http_.server_list_[i].listen_list_[j].getAddress() == addr &&
          config_->http_.server_list_[i].listen_list_[j].getport() == port) {
        tied_server.servers_.push_back(&config_->http_.server_list_[i]);
      }
    }
  }
  return tied_server;
}

std::string ConfigHandler::searchRootPath(const config::Server& server,
                                          const config::Location* location) const {
  if (location) {
    if (utils::hasDirective(*location, kRoot)) return location->root_.getPath();
    if (utils::hasDirective(*location, kAlias)) return location->alias_.getPath();
  }
  if (utils::hasDirective(server, kRoot))
    return server.root_.getPath();
  else if (utils::hasDirective(this->config_->http_, kRoot))
    return config_->http_.root_.getPath();
  // これいるか？上のやつと一緒でいいのでは？
  return config::Root::kDefaultPath_;
}

unsigned long ConfigHandler::searchCliMaxBodySize() const {
  const config::Size& size = this->config_->http_.client_max_body_size_.getSize();
  return size.size_in_bytes_;
}

bool ConfigHandler::isAutoIndexOn(const config::Server& server, const config::Location* location) const {
  if (location && utils::hasDirective(*location, kAutoindex)) {
    return location->autoindex_.getIsAutoindexOn();
  } else if (utils::hasDirective(server, kAutoindex)) {
    return server.autoindex_.getIsAutoindexOn();
  } else if (utils::hasDirective(config_->http_, kAutoindex) && config_->http_.autoindex_.getIsAutoindexOn())
    return true;
  return false;
}

std::string ConfigHandler::getCurrentTimeByLogFormat() const {
  std::time_t cur_time = std::time(NULL);
  std::tm* gmTime = std::gmtime(&cur_time);

  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << gmTime->tm_mday << "/" << months[gmTime->tm_mon] << "/"
      << 1900 + gmTime->tm_year << ":" << std::setfill('0') << std::setw(2) << gmTime->tm_hour << ":"
      << std::setfill('0') << std::setw(2) << gmTime->tm_min << ":" << std::setfill('0') << std::setw(2)
      << gmTime->tm_sec << " GMT";

  return oss.str();
}

std::string ConfigHandler::createAcsLogMsg(uint32_t ip, long status, size_t resSize,
                                           const HttpRequest& request) const {
  std::stringstream ss;

  std::string requestMethod, requestUrl, userAgent;

  requestMethod = config::LimitExcept::MethodToStr(request.method_);

  // URLの表示をするかどうか？
  std::map<std::string, std::string>::const_iterator it = request.headers_.find("User-Agent");
  if (it != request.headers_.end())
    userAgent = it->second;
  else
    userAgent = "-";

  ss << utils::ipToStr(ip) << " - - [" << getCurrentTimeByLogFormat() << "] \"" << requestMethod << " "
     << request.uri_ << " HTTP/1.1\" " << status << " " << resSize << " \"" << userAgent << "\"" << std::endl;

  return ss.str();
}

unsigned long ConfigHandler::getWorkerConnections() const {
  return config_->events_.worker_connections_.getWorkerConnections();
}

unsigned long ConfigHandler::getPollingMethod() const { return config_->events_.use_.getConnectionMethod(); }

std::string ConfigHandler::formatErrorLogMsg(const std::string& msg, config::LOG_LEVEL level) const {
  static const std::string& format = "%Y/%m/%d %H:%M:%S";
  std::time_t now = std::time(NULL);            // 現在のUNIX時刻を取得
  struct tm* local_now = std::localtime(&now);  // 現在時刻をローカルタイムに変換

  return utils::formatTm(local_now, format) + " [" + config::ErrorLog::LogLevelToStr(level) + "] " + msg +
         "\n";
}
