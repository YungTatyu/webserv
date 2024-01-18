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
		static unsigned int	num_; // event contextの出現回数
		const static bool	is_unique_ = true; // unique（重複不可）なcontextか
};
} // namespace config

#endif
