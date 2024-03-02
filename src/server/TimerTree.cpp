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

const TimerTree::timer_que	&TimerTree::getTimerTree() const
{
	return this->timer_que_;
}

const std::set<int>	&TimerTree::getFdSet() const
{
	return this->fd_set_;
}
