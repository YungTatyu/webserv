#ifndef CONFIG_AUTOINDEX_HPP
#define CONFIG_AUTOINDEX_HPP

#include "conf.hpp"

namespace config {
class Autoindex {
 private:
  bool is_autoindex_on_;

 public:
  Autoindex() : is_autoindex_on_(this->kDefaultIsAutoindexOn_) {}
  Autoindex(const Autoindex& other) { *this = other; }
  ~Autoindex() {}
  Autoindex& operator=(const Autoindex& other) {
    if (this != &other) {
      this->is_autoindex_on_ = other.is_autoindex_on_;
    }
    return *this;
  }
  bool getIsAutoindexOn() const { return this->is_autoindex_on_; }
  void setIsAutoindexOn(bool is_autoindex_on) { this->is_autoindex_on_ = is_autoindex_on; }
  static const bool kDefaultIsAutoindexOn_ = false;
  static const unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_NOT_UNIQUE;
};
}  // namespace config

#endif
