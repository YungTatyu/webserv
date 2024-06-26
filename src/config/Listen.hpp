#ifndef CONFIG_LISTEN_HPP
#define CONFIG_LISTEN_HPP

#include <string>

#include "conf.hpp"

namespace config {
class Listen {
 private:
  std::string address_;
  unsigned int port_;
  bool is_default_server_;

 public:
  Listen()
      : address_(this->kDefaultAddress_),
        port_(this->kDefaultPort_),
        is_default_server_(this->kDefaultIsDefaultServer_) {}
  Listen(const Listen &other) { *this = other; }
  ~Listen() {}
  Listen &operator=(const Listen &other) {
    if (this != &other) {
      this->address_ = other.address_;
      this->port_ = other.port_;
      this->is_default_server_ = other.is_default_server_;
    }
    return *this;
  }
  const static char *kDefaultAddress_;
  const static unsigned int kDefaultPort_ = 80;
  const static bool kDefaultIsDefaultServer_ = false;
  const std::string &getAddress() const { return this->address_; }
  const unsigned int &getport() const { return this->port_; }
  bool getIsDefaultServer() const { return this->is_default_server_; }
  void setAddress(const std::string &address) { this->address_ = address; }
  void setPort(unsigned int port) { this->port_ = port; }
  void setIsDefaultServer(bool is_default_server) { this->is_default_server_ = is_default_server; }
  const static unsigned int kType_ = CONF_HTTP_SERVER | CONF_TAKE123 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
