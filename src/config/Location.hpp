#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <set>
#include <string>
#include <vector>

#include "AccessLog.hpp"
#include "Alias.hpp"
#include "AllowDeny.hpp"
#include "Autoindex.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Index.hpp"
#include "KeepaliveTimeout.hpp"
#include "LimitExcept.hpp"
#include "ReceiveTimeout.hpp"
#include "Return.hpp"
#include "Root.hpp"
#include "SendTimeout.hpp"
#include "TryFiles.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"
#include "conf.hpp"

namespace config {
struct Location {
  Location(const std::string &uri) : uri_(uri) {}
  ~Location() {}
  const static unsigned int kType_ = CONF_HTTP_SERVER | CONF_TAKE1 | CONF_NOT_UNIQUE;
  std::set<std::string> directives_set_;
  const std::string uri_;
  LimitExcept limit_except_;
  std::vector<AccessLog> access_log_list_;
  std::vector<ErrorLog> error_log_list_;
  std::vector<ErrorPage> error_page_list_;
  Alias alias_;
  std::vector<AllowDeny> allow_deny_list_;
  std::vector<Index> index_list_;
  Autoindex autoindex_;
  KeepaliveTimeout keepalive_timeout_;
  ReceiveTimeout receive_timeout_;
  SendTimeout send_timeout_;
  std::vector<Return> return_list_;
  Root root_;
  TryFiles try_files_;
  Userid userid_;
  UseridDomain userid_domain_;
  UseridExpires userid_expires_;
  UseridPath userid_path_;
  UseridService userid_service_;
  std::vector<int> access_fd_list_;
  std::vector<int> error_fd_list_;
};
}  // namespace config

#endif
