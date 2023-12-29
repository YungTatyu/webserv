#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

namespace config
{
class Parser
{
	private:
		unsigned int	line_; // current line in file
	public:
		Parser();
		~Parser();
};
} // namespace config

#endif