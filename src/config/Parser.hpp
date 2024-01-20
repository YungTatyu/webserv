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
		CONTEXT	current_context_;
		bool	parseType();
		bool	expect(const config::TK_TYPE type);
		bool	is_context(const Token &token);
		bool	is_directive(const Token &token);
	public:
		Parser(const std::vector<Token> &tokens);
		~Parser();
		static std::set<std::string>	all_contexts_;
		static std::set<std::string>	all_directives_;
		bool	parse();
};
} // namespace config

#endif