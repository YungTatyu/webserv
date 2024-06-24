#ifndef CONFIG_USE_HPP
#define CONFIG_USE_HPP

#include "conf.hpp"

namespace config {
enum CONNECTION_METHOD {
  SELECT,
  POLL,
  KQUEUE,
  EPOLL,
};

class Use {
 private:
  CONNECTION_METHOD connection_method_;

 public:
  Use() {
#if defined(KQUEUE_AVAILABLE)
    this->connection_method_ = KQUEUE;
#elif defined(EPOLL_AVAILABLE)
    this->connection_method_ = EPOLL;
#else
    this->connection_method_ = POLL;
#endif
  }
  ~Use() {}
  CONNECTION_METHOD getConnectionMethod() const { return this->connection_method_; }
  void setConnectionMethod(const CONNECTION_METHOD connection_method) {
    this->connection_method_ = connection_method;
  }
  static std::string ConnectionMethodToStr(const config::CONNECTION_METHOD method) {
    std::string ret;
    switch (method) {
      case config::KQUEUE:
        ret = "kqueue";
        break;
      case config::EPOLL:
        ret = "epoll";
        break;
      case config::POLL:
        ret = "poll";
        break;
      case config::SELECT:
        ret = "select";
        break;
    }
    return ret;
  }
  const static unsigned int kType_ = CONF_EVENTS | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
