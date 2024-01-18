#ifndef CONFIG_LOCATION_PARSER_HPP
#define CONFIG_LOCATION_PARSER_HPP

namespace config
{
class LocationParser
{
	private:
		LocationParser();
	public:
		~LocationParser();
		static unsigned int	num_; // event contextの出現回数
		const static bool	is_unique_ = true; // unique（重複不可）なcontextか
};
} // namespace config

#endif
