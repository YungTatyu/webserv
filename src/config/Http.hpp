#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include <set>
#include <string>
#include <vector>

#include "AccessLog.hpp"
#include "AllowDeny.hpp"
#include "Autoindex.hpp"
#include "ClientMaxBodySize.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Index.hpp"
#include "KeepaliveTimeout.hpp"
#include "ReceiveTimeout.hpp"
#include "Root.hpp"
#include "SendTimeout.hpp"
#include "Server.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"

namespace config {
struct Http {
  const static unsigned int kType_ = CONF_MAIN | CONF_NOARGS | CONF_UNIQUE;
  std::set<std::string> directives_set_;
  std::vector<Server> server_list_;
  Root root_;
  std::vector<Index> index_list_;
  Autoindex autoindex_;
  ClientMaxBodySize client_max_body_size_;
  std::vector<AccessLog> access_log_list_;
  std::vector<ErrorLog> error_log_list_;
  std::vector<ErrorPage> error_page_list_;
  KeepaliveTimeout keepalive_timeout_;
  ReceiveTimeout receive_timeout_;
  SendTimeout send_timeout_;
  std::vector<AllowDeny> allow_deny_list_;
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
