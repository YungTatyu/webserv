#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <stack>
#include <string>
#include "conf.hpp"

namespace config
{
class Parser
{
	private:
		std::stack<CONTEXT>	contexts_stack_; // current contexts
		unsigned int	current_line_; // current line in file
	public:
		Parser();
		~Parser();
};
} // namespace config

#endif