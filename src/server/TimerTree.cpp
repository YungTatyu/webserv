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

std::multiset<Timer>::iterator	TimerTree::findTimerByFd(const int fd)
{
	(void)fd;
	return this->timer_tree_.begin();
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
