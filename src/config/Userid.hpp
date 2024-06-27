#ifndef CONFIG_USERID_HPP
#define CONFIG_USERID_HPP

namespace config {
class Userid {
 private:
  bool is_userid_on_;

 public:
  Userid() : is_userid_on_(this->kIsUseridOn_) {}
  Userid(const Userid& other) { *this = other; }
  ~Userid() {}
  Userid& operator=(const Userid& other) {
    if (this != &other) {
      this->is_userid_on_ = other.is_userid_on_;
    }
    return *this;
  }
  bool getIsUseridOn() const { return this->is_userid_on_; }
  void setIsUseridOn(bool is_userid_on) { this->is_userid_on_ = is_userid_on; }
  const static bool kIsUseridOn_ = false;
  const static unsigned int kType_ =
      CONF_HTTP | CONF_HTTP_SERVER | CONF_HTTP_LOCATION | CONF_TAKE1 | CONF_UNIQUE;
};
}  // namespace config

#endif
