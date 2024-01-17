#ifndef CONFIG_KEEPALIVE_TIMEOUT_HPP
#define CONFIG_KEEPALIVE_TIMEOUT_HPP

#include "Time.hpp"

namespace config
{
class KeepaliveTimeout
{
	private:
		Time	time_;
	public:
		KeepaliveTimeout();
		~KeepaliveTimeout();
		const Time	&getTime() const { return this->time_; }
		void	setTime(
			const unsigned int &d,
			const unsigned int &h,
			const unsigned int &m,
			const unsigned int &s,
			const unsigned int &ms
		)
		{ this->time_.setTime(d, h, m, s, ms); }
};
} // namespace config


#endif