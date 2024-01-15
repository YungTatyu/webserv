#ifndef CONFIG_TIME_HPP
#define CONFIG_TIME_HPP

namespace config
{
class Time
{
	private:
		unsigned int	d_; // days
		unsigned int	h_; // hours
		unsigned int	m_; // minutes
		unsigned int	s_; // seconds
		unsigned int	ms_; // milliseconds
	public:
		Time();
		~Time();
		void	set_time(
			const unsigned int &d,
			const unsigned int &h,
			const unsigned int &m,
			const unsigned int &s,
			const unsigned int &ms
		);
		const unsigned int	&get_time_in_seconds() const;

};
} // namespace config

#endif