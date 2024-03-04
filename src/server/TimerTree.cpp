#include "TimerTree.hpp"

#include <limits>

TimerTree::TimerTree() {}

TimerTree::~TimerTree() {}

void	TimerTree::addTimer(const Timer &timer)
{
	deleteTimer(timer.getFd());
	this->timer_tree_.insert(timer);
	this->fd_set_.insert(timer.getFd());
}

void	TimerTree::deleteTimer(const int fd)
{
	const std::set<int>::iterator	fd_it = this->fd_set_.find(fd);
	// fdが未登録の場合は何もしない
	if (fd_it == this->fd_set_.end())
		return;

	const std::multiset<Timer>::iterator	tree_it = findTimerByFd(fd);
	this->timer_tree_.erase(tree_it);
	this->fd_set_.erase(fd_it);
}

std::multiset<Timer>::iterator	TimerTree::findTimerByFd(const int fd)
{
	for (std::multiset<Timer>::iterator it = this->timer_tree_.begin();
		it != this->timer_tree_.end();
		++it
	)
	{
		if(it->getFd() == fd)
			return it;
	}
	return this->timer_tree_.end();
}

/**
 * @brief timeoutが存在するか
 * findTimer()を呼ぶ前に、この関数を呼ぶ必要がある
 * 
 * @return true 
 * @return false 
 */
bool	TimerTree::haveTimer() const
{
	return this->timer_tree_.size() != 0;
}

/**
 * @brief timeoutに一番近いnodeのtimeout値を返す
 * この関数を呼ぶ前に、haveTimer()を呼ぶ必要がある
 * 
 * @return int timeout
 */
int	TimerTree::findTimer() const
{
	const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
	// timerがない場合は、-1: timeoutなし
	if (it == this->timer_tree_.end())
		return -1;

	const unsigned long	timeout_raw = it->getTimeout() - Timer::getCurrentTime();
	// timeoutの値がintmaxを超えている場合は、intmaxを返す
	const int	timeout = timeout_raw > static_cast<unsigned long>(std::numeric_limits<int>::max()) ?
		std::numeric_limits<int>::max() : static_cast<int>(timeout_raw);
	return timeout;
}

const std::multiset<Timer>	&TimerTree::getTimerTree() const
{
	return this->timer_tree_;
}

const std::set<int>	&TimerTree::getFdSet() const
{
	return this->fd_set_;
}
