#ifndef CONFIG_EVENTS_PARSER_HPP
#define CONFIG_EVENTS_PARSER_HPP

namespace config
{
class EventsParser
{
	private:
		EventsParser();
	public:
		~EventsParser();
		static unsigned int	num_;
		const static bool	is_unique_ = true;
};
} // namespace config

#endif