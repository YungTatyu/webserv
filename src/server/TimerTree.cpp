#include "TimerTree.hpp"

TimerTree::TimerTree() {}

TimerTree::~TimerTree() {}


void	TimerTree::addTimer(const Timer &timer)
{
	(void)timer;
}

void	TimerTree::deleteTimer(const int fd)
{
	(void)fd;
}

int	TimerTree::findTimer() const
{
	return 1;
}

const std::multiset<Timer>	&TimerTree::getTimerTree() const
{
	return this->timer_tree_;
}


const std::set<int>	&TimerTree::getFdSet() const
{
	return this->fd_set_;
}
