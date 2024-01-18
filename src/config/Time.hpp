#ifndef CONFIG_TIME_HPP
#define CONFIG_TIME_HPP

#include <limits>

namespace config
{
class Time
{
	public:
		Time();
		~Time();
		unsigned long	time_in_ms_; // milliseconds
		const static unsigned int	seconds = 1000;
		const static unsigned int	minutes = seconds * 60;
		const static unsigned int	hours = minutes * 60;
		const static unsigned int	days = hours * 60;
		const static unsigned long	kMaxTimeInMilliseconds_ = std::numeric_limits<long>::max();;
};
} // namespace config

#endif