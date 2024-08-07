#include "TimerTree.hpp"

#include <sys/time.h>

#include <limits>

TimerTree::TimerTree() {}

TimerTree::TimerTree(const TimerTree &other) { *this = other; }

TimerTree::~TimerTree() {}

TimerTree &TimerTree::operator=(const TimerTree &other) {
  if (this != &other) {
    this->timer_tree_ = other.timer_tree_;
    this->fd_set_ = other.fd_set_;
  }
  return *this;
}

void TimerTree::addTimer(const Timer &timer) {
  deleteTimer(timer.getFd());
  this->timer_tree_.insert(timer);
  this->fd_set_.insert(timer.getFd());
}

void TimerTree::deleteTimer(int fd) {
  const std::set<int>::iterator fd_it = this->fd_set_.find(fd);
  // fdが未登録の場合は何もしない
  if (fd_it == this->fd_set_.end()) return;

  const std::multiset<Timer>::iterator tree_it = findTimerByFd(fd);
  this->timer_tree_.erase(tree_it);
  this->fd_set_.erase(fd_it);
}

std::multiset<Timer>::iterator TimerTree::findTimerByFd(int fd) {
  for (std::multiset<Timer>::iterator it = this->timer_tree_.begin(); it != this->timer_tree_.end(); ++it) {
    if (it->getFd() == fd) return it;
  }
  return this->timer_tree_.end();
}

/**
 * @brief timeoutに一番近いnodeのtimeout値を返す
 *
 * @return int timeout
 */
int TimerTree::findTimer() const {
  const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
  // timerがない場合は、-1: timeoutなし
  if (it == this->timer_tree_.end()) return -1;

  // この時点ですでにtimeoutになっている場合は、3msをtimeoutの値として設定する
  const unsigned long timeout_raw =
      it->getTimeout() > Timer::getCurrentTime() ? it->getTimeout() - Timer::getCurrentTime() : 3;
  // timeoutの値がintmaxを超えている場合は、intmaxを返す
  int timeout = timeout_raw >= static_cast<unsigned long>(std::numeric_limits<int>::max())
                    ? std::numeric_limits<int>::max()
                    : static_cast<int>(timeout_raw);
  return timeout;
}

struct timeval TimerTree::findTimeval() const {
  struct timeval tv;
  const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
  // timerがない場合は、-1: timeoutなし
  if (it == this->timer_tree_.end()) {
    tv.tv_sec = -1;
    tv.tv_usec = -1;
    return tv;
  }

  int timeout_raw = TimerTree::findTimer();

  tv.tv_sec = timeout_raw / 1000;
  tv.tv_usec = (timeout_raw % 1000) * 1000;
  return tv;
}

struct timespec TimerTree::findTimespec() const {
  struct timespec ts;
  const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
  // timerがない場合は、-1: timeoutなし
  if (it == this->timer_tree_.end()) {
    ts.tv_sec = -1;
    ts.tv_nsec = -1;
    return ts;
  }

  int timeout_raw = TimerTree::findTimer();
  ts.tv_sec = timeout_raw / 1000;
  ts.tv_nsec = (timeout_raw % 1000) * 1000000;
  return ts;
}

const std::multiset<Timer> &TimerTree::getTimerTree() const { return this->timer_tree_; }

const std::set<int> &TimerTree::getFdSet() const { return this->fd_set_; }

int TimerTree::getClosestTimeout() const {
  std::multiset<Timer>::iterator it = this->timer_tree_.begin();
  if (it == this->timer_tree_.end()) return -1;
  return it->getFd();
}

bool TimerTree::timerExists(int fd) const { return this->fd_set_.find(fd) != this->fd_set_.end(); }
