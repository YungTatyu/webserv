#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <set>
#include <string>
#include <vector>

#include "AccessLog.hpp"
#include "AllowDeny.hpp"
#include "Autoindex.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Index.hpp"
#include "KeepaliveTimeout.hpp"
#include "Listen.hpp"
#include "Location.hpp"
#include "ReceiveTimeout.hpp"
#include "Root.hpp"
#include "SendTimeout.hpp"
#include "ServerName.hpp"
#include "TryFiles.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"
#include "conf.hpp"

namespace config {
struct Server {
  const static unsigned int kType_ = CONF_HTTP | CONF_NOARGS | CONF_NOT_UNIQUE;
  std::set<std::string> directives_set_;
  std::vector<Location> location_list_;
  std::vector<AccessLog> access_log_list_;
  std::vector<ErrorLog> error_log_list_;
  std::vector<ErrorPage> error_page_list_;
  std::vector<AllowDeny> allow_deny_list_;
  std::vector<Index> index_list_;
  std::vector<Listen> listen_list_;
  Autoindex autoindex_;
  KeepaliveTimeout keepalive_timeout_;
  ReceiveTimeout receive_timeout_;
  SendTimeout send_timeout_;
  Root root_;
  ServerName server_name_;
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
