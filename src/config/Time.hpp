#ifndef CONFIG_TIME_HPP
#define CONFIG_TIME_HPP

#include <limits>

namespace config {
class Time {
 public:
  Time() : time_in_ms_(0) {}
  Time(unsigned long time_in_ms) : time_in_ms_(time_in_ms) {}
  Time(const Time& other) { *this = other; }
  ~Time() {}
  Time& operator=(const Time& other) {
    if (this != &other) {
      this->time_in_ms_ = other.time_in_ms_;
    }
    return *this;
  }
  bool isNoTime() const { return this->time_in_ms_ == 0; }
  unsigned long time_in_ms_;  // milliseconds
  static const unsigned int seconds = 1000;
  static const unsigned int minutes = seconds * 60;
  static const unsigned int hours = minutes * 60;
  static const unsigned int days = hours * 24;
  static const unsigned long kMaxTimeInMilliseconds_;
};
}  // namespace config

#endif
