#ifndef TIMER_HPP
#define TIMER_HPP

#include "Time.hpp"

class Timer {
 private:
  Timer();

  int fd_;
  unsigned long raw_time_ms_;  // directiveに設定されたraw value
  unsigned long timeout_ms_;   // timeoutの時間 = current_time_ + raw_time_ms_
  static unsigned long current_time_;

 public:
  enum TimeoutType {
    TMO_KEEPALIVE,
    TMO_RECV,
    TMO_SEND
  };
  Timer(int fd, const config::Time &time);
  Timer(const Timer &);
  ~Timer();
  Timer &operator=(const Timer &other);
  bool operator<(const Timer &other) const;
  bool operator>(const Timer &other) const;
  int getFd() const;
  unsigned long getRawtime() const;
  unsigned long getTimeout() const;
  void setTimeout(const config::Time &time);
  static unsigned long getCurrentTime();
  static void updateCurrentTime();
};

#endif
