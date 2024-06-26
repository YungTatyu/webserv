#ifndef CONFIG_CLIENT_MAX_BODY_SIZE
#define CONFIG_CLIENT_MAX_BODY_SIZE

#include "Size.hpp"
#include "conf.hpp"

namespace config {
class ClientMaxBodySize {
 private:
  Size size_;

 public:
  ClientMaxBodySize() : size_(this->kDefaultSize_) {}
  ClientMaxBodySize(const ClientMaxBodySize& other) { *this = other; }
  ~ClientMaxBodySize() {}
  ClientMaxBodySize& operator=(const ClientMaxBodySize& other) {
    if (this != &other) {
      this->size_ = other.size_;
    }
    return *this;
  }
  const Size& getSize() const { return this->size_; }
  void setSize(unsigned long size_in_bytes) { this->size_.size_in_bytes_ = size_in_bytes; }
  const static unsigned long kDefaultSize_ = Size::megabytes;  // 1m
  const static unsigned int kType_ = CONF_HTTP | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
