#ifndef CONFIG_MAIN_PARSER_HPP
#define CONFIG_MAIN_PARSER_HPP

namespace config
{
class MainParser
{
	private:
		MainParser();
	public:
		~MainParser();
		const static bool	is_unique_ = true; // unique（重複不可）なcontextか
};
} // namespace config

#endif
