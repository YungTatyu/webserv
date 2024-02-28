#ifndef TIMER_TREE_HPP
#define TIMER_TREE_HPP

#include <set>

#include "Timer.hpp"

class TimerTree
{
	private:
		std::multiset<Timer>	timer_tree_;
		std::set<int>	fd_set_; // treeに登録されているfdを管理する
		std::multiset<Timer>::iterator	findTimerByFd();
	public:
		TimerTree();
		~TimerTree();
		void	addTimer(const Timer &timer);
		void	deleteTimer(const Timer &timer);
		int	findTimer() const; // timeoutに一番近いnode
		const std::multiset<Timer>	&getTimerTree() const;
};

#endif
