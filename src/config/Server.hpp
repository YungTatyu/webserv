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
  const static unsigned int type = CONF_HTTP | CONF_NOARGS | CONF_NOT_UNIQUE;
  std::set<std::string> directives_set;
  std::vector<Location> location_list;
  std::vector<AccessLog> access_log_list;
  std::vector<ErrorLog> error_log_list;
  std::vector<ErrorPage> error_page_list;
  std::vector<AllowDeny> allow_deny_list;
  std::vector<Index> index_list;
  std::vector<Listen> listen_list;
  Autoindex autoindex;
  KeepaliveTimeout keepalive_timeout;
  ReceiveTimeout receive_timeout;
  SendTimeout send_timeout;
  Root root;
  ServerName server_name;
  TryFiles try_files;
  Userid userid;
  UseridDomain userid_domain;
  UseridExpires userid_expires;
  UseridPath userid_path;
  UseridService userid_service;
  std::vector<int> access_fd_list;
  std::vector<int> error_fd_list;
};
}  // namespace config

#endif
