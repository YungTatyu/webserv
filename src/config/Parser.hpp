#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <set>
#include <string>

#include "conf.hpp"
#include "Lexer.hpp"

namespace config
{
class Parser
{
	private:
		const std::vector<Token>	&tokens_;
		std::set<std::string>	set_contexts_;
		size_t	ti; // token index
		// std::stack<CONTEXT>	contexts_stack_; // current contexts
		CONTEXT	current_context_;
	public:
		Parser(const std::vector<Token> &tokens);
		~Parser();
		bool	expect(const config::TK_TYPE type);
		static std::set<std::string>	all_contexts_;
		static std::set<std::string>	all_directives_;
};
} // namespace config

#endif