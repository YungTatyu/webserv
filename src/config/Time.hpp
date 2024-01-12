#ifndef CONFIG_TIME_HPP
#define CONFIG_TIME_HPP

namespace config
{
class Time
{
	private:
		unsigned int	d; // days
		unsigned char	h; // hours
		unsigned char	m; // minutes
		unsigned char	s; // seconds
		unsigned int	ms; // milliseconds
	public:
		Time();
		~Time();
};	
} // namespace config

#endif