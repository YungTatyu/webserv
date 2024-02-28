#include "Timer.hpp"

#include <sys/time.h>
#include <stdio.h>
#include <iostream>

Timer::Timer(const int fd, const config::Time &time) : fd_(fd)
{
	this->raw_time_ms_ = time.time_in_ms_;
}

Timer::~Timer() {}

unsigned long	Timer::getCurrentTime() const
{
	struct timeval	t;

	if (gettimeofday(&t, NULL) == -1)
	{
		std::cerr << "webserv: [emerg] gettimeofday() failed (" << errno << ": " << strerror(errno) << ")\n";
		return 0;
	}
	return (t.tv_sec * 1000) + (t.tv_usec / 1000);
}
