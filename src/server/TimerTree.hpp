#ifndef TIMER_TREE_HPP
#define TIMER_TREE_HPP

#include <set>
#include <queue>

#include "Timer.hpp"

class TimerTree
{
	private:
		// std::multiset<Timer>	timer_tree_;
		std::priority_queue<
			Timer,
			std::vector<Timer>,
			std::greater<Timer>
		>timer_que_;
		std::set<int>	fd_set_; // treeに登録されているfdを管理する
		std::multiset<Timer>::iterator	findTimerByFd();
	public:
		TimerTree();
		~TimerTree();
		void	addTimer(const Timer &timer);
		void	deleteTimer(const int fd);
		int	findTimer() const; // timeoutに一番近いnode
		const std::multiset<Timer>	&getTimerTree() const;
		const std::set<int>	&getFdSet() const;
};

#endif
