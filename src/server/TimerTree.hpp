#ifndef TIMER_TREE_HPP
#define TIMER_TREE_HPP

#include <set>
#include <queue>

#include "Timer.hpp"

class TimerTree
{
	public:
		typedef std::priority_queue<
			const Timer,
			std::vector<const Timer>,
			std::greater<const Timer>
		> timer_que;
		TimerTree();
		~TimerTree();
		void	addTimer(const Timer &timer);
		void	deleteTimer(const int fd);
		int	findTimer() const; // timeoutに一番近いnode
		// const std::multiset<Timer>	&getTimerTree() const;
		const timer_que	&getTimerTree() const;
		const std::set<int>	&getFdSet() const;
	private:
		// std::multiset<Timer>	timer_tree_;
		timer_que timer_que_;
		std::set<int>	fd_set_; // treeに登録されているfdを管理する
		std::multiset<Timer>::iterator	findTimerByFd();
};

#endif
