#ifndef CONFIG_TIME_HPP
#define CONFIG_TIME_HPP

#include <ctime>

namespace config
{
class Time
{
	private:
		time_t	s; // seconds
		unsigned int	ms; // milliseconds
	public:
		Time();
		~Time();
};	
} // namespace config

#endif