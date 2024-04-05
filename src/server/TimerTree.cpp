#include "TimerTree.hpp"

#include <sys/time.h>
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
 * @brief timeoutに一番近いnodeのtimeout値を返す
 * 
 * @return int timeout
 */
int	TimerTree::findTimer() const
{
	const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
	// timerがない場合は、-1: timeoutなし
	if (it == this->timer_tree_.end())
		return -1;

	// この時点ですでにtimeoutになっている場合は、3msをtimeoutの値として設定する
	const unsigned long	timeout_raw = it->getTimeout() > Timer::getCurrentTime() ? it->getTimeout() - Timer::getCurrentTime() : 3;
	// timeoutの値がintmaxを超えている場合は、intmaxを返す
	const int	timeout = timeout_raw >= static_cast<unsigned long>(std::numeric_limits<int>::max()) ?
		std::numeric_limits<int>::max() : static_cast<int>(timeout_raw);
	return timeout;
}

struct timeval	TimerTree::findTimeval() const
{
	struct timeval	tv;
	const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
	// timerがない場合は、-1: timeoutなし
	if (it == this->timer_tree_.end())
	{
		tv.tv_sec = -1;
		tv.tv_usec = -1;
	}

	// この時点ですでにtimeoutになっている場合は、3msをtimeoutの値として設定する
	const unsigned long	timeout_raw = it->getTimeout() > Timer::getCurrentTime() ? it->getTimeout() - Timer::getCurrentTime() : 3;
	// timeoutの値がintmaxを超えている場合は、intmaxを返す
	tv.tv_sec = timeout_raw >= static_cast<unsigned long>(std::numeric_limits<int>::max()) ?
		std::numeric_limits<int>::max() / 1000 : static_cast<int>(timeout_raw) / 1000;
	tv.tv_usec = timeout_raw >= static_cast<unsigned long>(std::numeric_limits<int>::max()) ?
		(std::numeric_limits<int>::max() % 1000) * 1000 : (static_cast<int>(timeout_raw) % 1000) * 1000;
	return tv;
}

struct timespec	TimerTree::findTimespec() const
{
	struct timespec	ts;
	const std::multiset<Timer>::iterator it = this->timer_tree_.begin();
	// timerがない場合は、-1: timeoutなし
	if (it == this->timer_tree_.end())
	{
		ts.tv_sec = -1;
		ts.tv_nsec = -1;
	}

	// この時点ですでにtimeoutになっている場合は、3msをtimeoutの値として設定する
	const unsigned long	timeout_raw = it->getTimeout() > Timer::getCurrentTime() ? it->getTimeout() - Timer::getCurrentTime() : 3;
	// timeoutの値がintmaxを超えている場合は、intmaxを返す
	ts.tv_sec = timeout_raw >= static_cast<unsigned long>(std::numeric_limits<int>::max()) ?
		std::numeric_limits<int>::max() / 1000 : static_cast<int>(timeout_raw) / 1000;
	ts.tv_nsec = timeout_raw >= static_cast<unsigned long>(std::numeric_limits<int>::max()) ?
		(std::numeric_limits<int>::max() % 1000) * 1000 : (static_cast<int>(timeout_raw) % 1000) * 1000000;
	return ts;
}

const std::multiset<Timer>	&TimerTree::getTimerTree() const
{
	return this->timer_tree_;
}

const std::set<int>	&TimerTree::getFdSet() const
{
	return this->fd_set_;
}


bool	TimerTree::timerExists(const int fd) const
{
	for (std::multiset<Timer>::iterator it = this->timer_tree_.begin();
		it != this->timer_tree_.end();
		++it
	)
	{
		if(it->getFd() == fd)
			return true;
	}
	return false;
}
