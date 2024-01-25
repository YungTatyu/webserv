#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <set>
#include <map>
#include <string>

#include "conf.hpp"
#include "Lexer.hpp"
#include "Main.hpp"

namespace config
{
class Parser
{
	private:
		Main	config_;
		const std::vector<Token>	&tokens_;
		std::set<std::string>	set_directives_; // setされたcontextとdirectiveを管理
		const std::string	filepath_;
		size_t	ti; // token index
		CONTEXT	current_context_;
		std::map<std::string, bool (config::Parser::*)()>	parser_map_;
		bool	parseType(const Token &token);
		bool	expectTokenType(const config::TK_TYPE type, const Token &token) const;
		bool	expectArgsNum(const unsigned int expect, const unsigned int actual) const;
		bool	isContext(const Token &token) const;
		bool	isDirective(const Token &token) const;
		const std::set<std::string>	*searchDirectivesSet(const CONTEXT context) const;
		void	printError(const std::string &err_msg, const Token &token) const;
		ssize_t	countArgs(const TK_TYPE terminating_token) const;
		bool	parseAccessLog();
		Parser();
	public:
		Parser(const std::vector<Token> &tokens, const std::string &filepath);
		~Parser();
		static std::map<std::string, unsigned int>	all_directives_; // すべてのcontextとdirectiveの情報を管理
 		bool	parse();
};
} // namespace config

#endif