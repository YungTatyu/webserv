#include "Timer.hpp"

#include <sys/time.h>
#include <stdio.h>
#include <iostream>

Timer::Timer(const int fd, const config::Time &time) : fd_(fd)
{
	setTimeout(time);
}

Timer::~Timer() {}

unsigned long	Timer::getCurrentTime() const
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000) + (t.tv_usec / 1000);
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
