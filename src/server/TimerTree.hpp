#ifndef TIMER_TREE_HPP
#define TIMER_TREE_HPP

#include <set>

#include "Timer.hpp"

class TimerTree {
 private:
  std::multiset<Timer>::iterator findTimerByFd(int fd);

  std::multiset<Timer> timer_tree_;
  std::set<int> fd_set_;  // treeに登録されているfdを管理する

 public:
  TimerTree();
  TimerTree(const TimerTree &);
  ~TimerTree();
  TimerTree &operator=(const TimerTree &);
  void addTimer(const Timer &timer);
  void deleteTimer(int fd);
  int findTimer() const;
  struct timeval findTimeval() const;
  struct timespec findTimespec() const;
  const std::multiset<Timer> &getTimerTree() const;
  const std::set<int> &getFdSet() const;
  int getClosestTimeout() const;
  bool timerExists(int fd) const;
};

#endif
