#include "ConfigHandler.hpp"

#include <sys/socket.h>

#include <cerrno>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <ostream>

#include "NetworkIOHandler.hpp"
#include "Utils.hpp"
#include "WebServer.hpp"
#include "error.hpp"

const static std::string kACCESS_FD = "access_fd";
const static std::string kERROR_FD = "error_fd";
const static std::string kKEEPALIVE_TIMEOUT = "keepalive_timeout";
const static std::string kRECEIVE_TIMEOUT = "receive_timeout";
const static std::string kSEND_TIMEOUT = "send_timeout";
const static std::string kUSERID_EXPIRES = "userid_expires";
const static std::string kDENY = "deny";
const static std::string kALIAS = "alias";
const static std::string kAUTOINDEX = "autoindex";
const static std::string kROOT = "root";
const static std::string kERROR_PAGE = "error_page";
const static std::string kLimitExcept = "limit_except";

/** Configにあってほしい機能
 * デフォルトサーバがどれか
 *  ipとmask分けて保存
 */

/* ConfigHandlerクラスの実装 */
void ConfigHandler::loadConfiguration(const config::Main* config) {
  /* ConfファイルをパースしてデータをServConfigクラスにセットする */
  this->config_ = config;
}

bool ConfigHandler::addressInLimit(const std::string& ip_addr_str, const uint32_t cli_addr) const {
  if (ip_addr_str == "all") return true;
  std::istringstream iss(ip_addr_str);
  std::string ip;
  std::string mask;

  std::getline(iss, ip, '/');
  std::getline(iss, mask);

  uint32_t conf_addr = Utils::StrToIPAddress(ip);
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
                              const uint32_t cli_addr) const {
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
  if (location && Utils::hasDirective(*location, kDENY)) {
    if (!limitLoop(location->allow_deny_list, client_addr.sin_addr.s_addr)) return ACCESS_DENY;
  } else if (Utils::hasDirective(server, kDENY)) {
    if (!limitLoop(server.allow_deny_list, client_addr.sin_addr.s_addr)) return ACCESS_DENY;
  } else if (Utils::hasDirective(this->config_->http, kDENY)) {
    if (!limitLoop(this->config_->http.allow_deny_list, client_addr.sin_addr.s_addr)) return ACCESS_DENY;
  }

  // ------ method の制限 ------
  // location内にlimit_except contextあるか？
  if (location && Utils::hasDirective(*location, kLimitExcept)) {
    // 制限されたメソッドでなければ、スルー
    // HttpRequestでLIMIT_EXCEPTのenum使ってほしい
    if (location->limit_except.excepted_methods.find(request.method) ==
        location->limit_except.excepted_methods.end()) {
      if (!limitLoop(location->limit_except.allow_deny_list, client_addr.sin_addr.s_addr)) return METHOD_DENY;
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
  // access_logがどのコンテキスがあれば出力する
  if (location && Utils::hasDirective(*location, kACCESS_FD)) {
    for (size_t i = 0; i < location->access_fd_list.size(); i++) {
      if (Utils::wrapperWrite(location->access_fd_list[i], msg) == -1)
        WebServer::writeErrorlog(error::strSysCallError("write") + "\n");
    }
  } else if (Utils::hasDirective(server, kACCESS_FD)) {
    for (size_t i = 0; i < server.access_fd_list.size(); i++) {
      if (Utils::wrapperWrite(server.access_fd_list[i], msg) == -1)
        WebServer::writeErrorlog(error::strSysCallError("write") + "\n");
    }
  } else if (Utils::hasDirective(this->config_->http, kACCESS_FD)) {
    for (size_t i = 0; i < this->config_->http.access_fd_list.size(); i++) {
      if (Utils::wrapperWrite(this->config_->http.access_fd_list[i], msg) == -1)
        WebServer::writeErrorlog(error::strSysCallError("write") + "\n");
    }
  }
}

void ConfigHandler::writeErrorLog(const std::string& msg) const {
  // http conterxtにerror_logディレクティブがあれば出力
  if (Utils::hasDirective(this->config_->http, kERROR_FD)) {
    for (size_t i = 0; i < this->config_->http.error_fd_list.size(); i++) {
      if (Utils::wrapperWrite(this->config_->http.error_fd_list[i], msg) == -1)
        std::cerr << error::strSysCallError("write") << std::endl;
    }
  } else if (Utils::hasDirective(*this->config_, kERROR_FD)) {
    // main contextにerror_logディレクティブがなくてもデフォルトに出力する
    // fdがopenできずに追加できていない可能性があるので、一応条件文で確認している。
    for (size_t i = 0; i < this->config_->error_fd_list.size(); i++) {
      if (Utils::wrapperWrite(this->config_->error_fd_list[i], msg) == -1)
        std::cerr << error::strSysCallError("write") << std::endl;
    }
  }
}

void ConfigHandler::writeErrorLog(const struct TiedServer& tied_servers, const std::string& server_name,
                                  const std::string& uri, const std::string& msg) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);
  writeErrorLog(server, location, msg);
}

void ConfigHandler::writeErrorLog(const config::Server& server, const config::Location* location,
                                  const std::string& msg) const {
  if (location && Utils::hasDirective(*location, kERROR_FD)) {
    for (size_t i = 0; i < location->error_fd_list.size(); i++) {
      if (Utils::wrapperWrite(location->error_fd_list[i], msg) == -1)
        std::cerr << error::strSysCallError("write") << std::endl;
    }
  } else if (Utils::hasDirective(server, kERROR_FD)) {
    for (size_t i = 0; i < server.error_fd_list.size(); i++) {
      if (Utils::wrapperWrite(server.error_fd_list[i], msg) == -1)
        std::cerr << error::strSysCallError("write") << std::endl;
    }
  } else if (Utils::hasDirective(this->config_->http, kERROR_FD)) {
    for (size_t i = 0; i < this->config_->http.error_fd_list.size(); i++) {
      if (Utils::wrapperWrite(this->config_->http.error_fd_list[i], msg) == -1)
        std::cerr << error::strSysCallError("write") << std::endl;
    }
  } else if (Utils::hasDirective(*this->config_, kERROR_FD)) {
    for (size_t i = 0; i < this->config_->error_fd_list.size(); i++) {
      if (Utils::wrapperWrite(this->config_->error_fd_list[i], msg) == -1)
        std::cerr << error::strSysCallError("write") << std::endl;
    }
  }
}

const config::Time& ConfigHandler::searchKeepaliveTimeout(const struct TiedServer& tied_servers,
                                                          const std::string& server_name,
                                                          const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && Utils::hasDirective(*location, kKEEPALIVE_TIMEOUT)) {
    return location->keepalive_timeout.getTime();
  } else if (Utils::hasDirective(server, kKEEPALIVE_TIMEOUT)) {
    return server.keepalive_timeout.getTime();
  } else  // http兼default
  {
    return this->config_->http.keepalive_timeout.getTime();
  }
}

const config::Time& ConfigHandler::searchReceiveTimeout(const struct TiedServer& tied_servers,
                                                        const std::string& server_name,
                                                        const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && Utils::hasDirective(*location, kRECEIVE_TIMEOUT)) {
    return location->receive_timeout.getTime();
  } else if (Utils::hasDirective(server, kRECEIVE_TIMEOUT)) {
    return server.receive_timeout.getTime();
  }
  // http兼default
  return this->config_->http.receive_timeout.getTime();
}

const config::Time& ConfigHandler::searchSendTimeout(const TiedServer& tied_servers,
                                                     const std::string& server_name,
                                                     const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && Utils::hasDirective(*location, kSEND_TIMEOUT)) {
    return location->send_timeout.getTime();
  } else if (Utils::hasDirective(server, kSEND_TIMEOUT)) {
    return server.send_timeout.getTime();
  } else  // http兼default
  {
    return this->config_->http.send_timeout.getTime();
  }
}

const config::Time& ConfigHandler::searchUseridExpires(const struct TiedServer& tied_servers,
                                                       const std::string& server_name,
                                                       const std::string& uri) const {
  const config::Server& server = searchServerConfig(tied_servers, server_name);
  const config::Location* location = searchLongestMatchLocationConfig(server, uri);

  if (location && Utils::hasDirective(*location, kUSERID_EXPIRES)) {
    return location->userid_expires.getTime();
  } else if (Utils::hasDirective(server, kUSERID_EXPIRES)) {
    return server.userid_expires.getTime();
  } else  // http兼default
  {
    return this->config_->http.userid_expires.getTime();
  }
}

const config::Server& ConfigHandler::searchServerConfig(const struct TiedServer& tied_servers,
                                                        const std::string& server_name) const {
  const config::Server* default_server = tied_servers.servers_[0];

  for (size_t si = 0; si < tied_servers.servers_.size(); si++) {
    if (server_name != "" && tied_servers.servers_[si]->server_name.getName().find(server_name) !=
                                 tied_servers.servers_[si]->server_name.getName().end())
      return *tied_servers.servers_[si];
    for (size_t li = 0; li < tied_servers.servers_[si]->listen_list.size(); li++) {
      const config::Listen& tmp_listen = tied_servers.servers_[si]->listen_list[li];
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

  for (size_t i = 0; i < server_config.location_list.size(); i++) {
    std::string config_uri = server_config.location_list[i].uri;
    if (uri.find(config_uri) == 0 && max_len < config_uri.size()) {
      max_len = std::max(max_len, config_uri.size());
      longest_match = &server_config.location_list[i];
    }
  }
  return longest_match;
}

const config::ErrorPage* ConfigHandler::searchErrorPage(const config::Server& server,
                                                        const config::Location* location,
                                                        const unsigned int code) const {
  if (location && Utils::hasDirective(*location, kERROR_PAGE)) {
    const std::vector<config::ErrorPage>& ep_list = location->error_page_list;
    for (size_t i = 0; i < ep_list.size(); i++) {
      if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end()) {
        return &ep_list[i];
      }
    }
  } else if (Utils::hasDirective(server, kERROR_PAGE)) {
    const std::vector<config::ErrorPage>& ep_list = server.error_page_list;
    for (size_t i = 0; i < ep_list.size(); i++) {
      if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end()) {
        return &ep_list[i];
      }
    }
  } else if (Utils::hasDirective(this->config_->http, kERROR_PAGE)) {
    const std::vector<config::ErrorPage>& ep_list = this->config_->http.error_page_list;
    for (size_t i = 0; i < ep_list.size(); i++) {
      if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end()) {
        return &ep_list[i];
      }
    }
  }

  return NULL;
}

struct TiedServer ConfigHandler::createTiedServer(const std::string addr, const unsigned int port) const {
  struct TiedServer tied_server(addr, port);

  for (size_t i = 0; i < config_->http.server_list.size(); i++) {
    for (size_t j = 0; j < config_->http.server_list[i].listen_list.size(); j++) {
      if (config_->http.server_list[i].listen_list[j].getAddress() == addr &&
          config_->http.server_list[i].listen_list[j].getport() == port) {
        tied_server.servers_.push_back(&config_->http.server_list[i]);
      }
    }
  }
  return tied_server;
}

std::string ConfigHandler::searchRootPath(const config::Server& server,
                                          const config::Location* location) const {
  if (location) {
    if (Utils::hasDirective(*location, kROOT)) return location->root.getPath();
    if (Utils::hasDirective(*location, kALIAS)) return location->alias.getPath();
  }
  if (Utils::hasDirective(server, kROOT))
    return server.root.getPath();
  else if (Utils::hasDirective(this->config_->http, kROOT))
    return config_->http.root.getPath();
  // これいるか？上のやつと一緒でいいのでは？
  return config::Root::kDefaultPath_;
}

unsigned long ConfigHandler::searchCliMaxBodySize() const {
  const config::Size& size = this->config_->http.client_max_body_size.getSize();
  return size.size_in_bytes_;
}

bool ConfigHandler::isAutoIndexOn(const config::Server& server, const config::Location* location) const {
  if (location && Utils::hasDirective(*location, kAUTOINDEX)) {
    return location->autoindex.getIsAutoindexOn();
  } else if (Utils::hasDirective(server, kAUTOINDEX)) {
    return server.autoindex.getIsAutoindexOn();
  } else if (Utils::hasDirective(config_->http, kAUTOINDEX) && config_->http.autoindex.getIsAutoindexOn())
    return true;
  return false;
}

const std::string ConfigHandler::getCurrentTimeByLogFormat() const {
  std::time_t currentTime = std::time(NULL);
  std::tm* gmTime = std::gmtime(&currentTime);

  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << gmTime->tm_mday << "/" << months[gmTime->tm_mon] << "/"
      << 1900 + gmTime->tm_year << ":" << std::setfill('0') << std::setw(2) << gmTime->tm_hour << ":"
      << std::setfill('0') << std::setw(2) << gmTime->tm_min << ":" << std::setfill('0') << std::setw(2)
      << gmTime->tm_sec << " GMT";

  return oss.str();
}

std::string ConfigHandler::createAcsLogMsg(const uint32_t ip, const long status, const size_t resSize,
                                           const HttpRequest& request) const {
  std::stringstream ss;

  std::string requestMethod, requestUrl, userAgent;

  requestMethod = config::LimitExcept::MethodToStr(request.method);

  // URLの表示をするかどうか？
  std::map<std::string, std::string>::const_iterator it = request.headers.find("User-Agent");
  if (it != request.headers.end())
    userAgent = it->second;
  else
    userAgent = "-";

  ss << Utils::ipToStr(ip) << " - - [" << getCurrentTimeByLogFormat() << "] \"" << requestMethod << " "
     << request.uri << " HTTP/1.1\" " << status << " " << resSize << " \"" << userAgent << "\"" << std::endl;

  return ss.str();
}

unsigned long ConfigHandler::getWorkerConnections() const {
  return config_->events.worker_connections.getWorkerConnections();
}
