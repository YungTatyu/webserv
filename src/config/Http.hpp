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
  Http() {}
  Http(const Http& other) { *this = other; }
  ~Http() {}
  Http& operator=(const Http& other) {
    if (this != &other) {
      this->directives_set_ = other.directives_set_;
      this->server_list_ = other.server_list_;
      this->root_ = other.root_;
      this->index_list_ = other.index_list_;
      this->autoindex_ = other.autoindex_;
      this->client_max_body_size_ = other.client_max_body_size_;
      this->access_log_list_ = other.access_log_list_;
      this->error_log_list_ = other.error_log_list_;
      this->error_page_list_ = other.error_page_list_;
      this->keepalive_timeout_ = other.keepalive_timeout_;
      this->receive_timeout_ = other.receive_timeout_;
      this->send_timeout_ = other.send_timeout_;
      this->allow_deny_list_ = other.allow_deny_list_;
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
