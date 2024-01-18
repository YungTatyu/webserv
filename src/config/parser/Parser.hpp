#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <stack>
#include <string>
#include "conf.hpp"

namespace config
{

class ParseState
{
}

class Parser
{
	private:
		std::string	file_content_;
		CONTEXT	current_context_;
		// std::stack<CONTEXT>	contexts_stack_; // current contexts
		unsigned int	current_line_; // current line in file
	public:
		Parser();
		~Parser();
};
} // namespace config

#endif
