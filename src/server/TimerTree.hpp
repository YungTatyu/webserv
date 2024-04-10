#ifndef TIMER_TREE_HPP
#define TIMER_TREE_HPP

#include <set>

#include "Timer.hpp"

class TimerTree
{
	private:
		std::multiset<Timer>	timer_tree_;
		std::set<int>	fd_set_; // treeに登録されているfdを管理する
		std::multiset<Timer>::iterator	findTimerByFd(const int fd);
	public:
		TimerTree();
		~TimerTree();
		void	addTimer(const Timer &timer);
		void	deleteTimer(const int fd);
		int	findTimer() const;
		struct timeval	findTimeval() const;
		struct timespec	findTimespec() const;
		const std::multiset<Timer>	&getTimerTree() const;
		const std::set<int>	&getFdSet() const;
		bool	timerExists(const int fd) const;
};

#endif

/* Timerを設定・更新するタイミング
 * KeepAliveTimeout: ReadEventに変えるとき。
 *
 *
 * Timerを消すタイミング
 * KeepAliveTimeout: receiveする前、handleErrorEvent
 *
 */
