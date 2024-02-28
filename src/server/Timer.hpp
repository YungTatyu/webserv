#ifndef TIMER_HPP
#define TIMER_HPP

#include "Time.hpp"

class Timer
{
	private:
		const int	fd_;
		unsigned long	raw_time_ms_; // directiveに設定されたraw value
		unsigned long	timeout_ms_; // timeoutの時間 = now + raw_time_ms_
		Timer();
	public:
		Timer(const int fd, const config::Time &time);
		~Timer();
		bool	operator<(const Timer &other) const;
		int	getFd() const;
		unsigned long	getRawtime() const;
		unsigned long	getTimeout() const;
		void	setTimeout(const config::Time &time);
		unsigned long	getCurrentTime() const;
};

#endif
