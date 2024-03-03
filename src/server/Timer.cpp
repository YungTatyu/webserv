#include "Timer.hpp"

#include <sys/time.h>
#include <stdio.h>
#include <iostream>

unsigned long	Timer::current_time_;

Timer::Timer(const int fd, const config::Time &time) : fd_(fd)
{
	setTimeout(time);
}

Timer::~Timer() {}

/**
 * @brief getCurrentTime()を呼ぶたびに計算の処理をしたくない
 * getCurrentTime()を呼ぶ前に、この関数を読ぶ
 * 
 */
void	Timer::updateCurrentTime()
{
	// struct timeval	t;

	// gettimeofday(&t, NULL);
	// return (t.tv_sec * 1000) + (t.tv_usec / 1000);

	/**
	 * nginxでは、
	 * gettimeofday()の後に、clock_gettime(CLOCK_MONOTONIC, &ts)でmsをupdateしている
	 * gettimeofday(): 日付と時刻の取得
	 * clock_gettime(): システム起動以降の経過時間を取得でき、システム時刻の変更に影響を受けない
	 * 
	 */
	struct timespec  ts;
	#if defined(CLOCK_MONOTONIC_FAST)
		clock_gettime(CLOCK_MONOTONIC_FAST, &ts);
	#else
		clock_gettime(CLOCK_MONOTONIC, &ts);
	#endif

	time_t	sec = ts.tv_sec;
	long	msec = ts.tv_nsec / 1000000;
	current_time_ = (sec * 1000) + msec;
}

/**
 * @brief この関数を呼ぶ前に、updateCurrentTime()を呼んで、
 * current_timeを初期化する必要がある（毎回初期化する必要はない）
 * timeoutの処理をする前に、一度だけcurrent_timeをupdateする
 * 
 * @return unsigned long 
 */
unsigned long	Timer::getCurrentTime()
{
	return current_time_;
}

int	Timer::getFd() const
{
	return this->fd_;
}

unsigned long	Timer::getRawtime() const
{
	return this->raw_time_ms_;
}

unsigned long	Timer::getTimeout() const
{
	return this->timeout_ms_;
}

void	Timer::setTimeout(const config::Time &time)
{
	this->raw_time_ms_ = time.time_in_ms_;
	this->timeout_ms_ = time.time_in_ms_ + getCurrentTime();
}

bool	Timer::operator<(const Timer &other) const
{
	return this->timeout_ms_ < other.timeout_ms_;
}

bool	Timer::operator>(const Timer &other) const
{
	return this->timeout_ms_ > other.timeout_ms_;
}
