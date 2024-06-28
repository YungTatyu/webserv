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
  Server() {}
  Server(const Server& other) { *this = other; }
  ~Server() {}
  Server& operator=(const Server& other) {
    if (this != &other) {
      this->directives_set_ = other.directives_set_;
      this->location_list_ = other.location_list_;
      this->access_log_list_ = other.access_log_list_;
      this->error_log_list_ = other.error_log_list_;
      this->error_page_list_ = other.error_page_list_;
      this->allow_deny_list_ = other.allow_deny_list_;
      this->index_list_ = other.index_list_;
      this->listen_list_ = other.listen_list_;
      this->autoindex_ = other.autoindex_;
      this->keepalive_timeout_ = other.keepalive_timeout_;
      this->receive_timeout_ = other.receive_timeout_;
      this->send_timeout_ = other.send_timeout_;
      this->root_ = other.root_;
      this->server_name_ = other.server_name_;
      this->try_files_ = other.try_files_;
      this->userid_ = other.userid_;
      this->userid_domain_ = other.userid_domain_;
      this->userid_expires_ = other.userid_expires_;
      this->userid_path_ = other.userid_path_;
      this->userid_service_ = other.userid_service_;
      this->access_fd_list_ = other.access_fd_list_;
      this->error_fd_list_ = other.error_fd_list_;
    }
    return *this;
  }
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
  static const unsigned int kType_ = CONF_HTTP | CONF_NOARGS | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
