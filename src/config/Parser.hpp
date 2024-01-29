#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <set>
#include <map>
#include <stack>
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
		const std::string	filepath_;
		size_t	ti; // token index
		std::stack<CONTEXT>	current_context_;
		std::map<std::string, bool (config::Parser::*)()>	parser_map_;
		bool	parseType(const Token &token);
		bool	expectTokenType(const config::TK_TYPE type, const Token &token) const;
		bool	expectArgsNum(const unsigned int expect, const unsigned int actual) const;
		bool	expectTerminatingToken() const;
		bool	isContext(const Token &token) const;
		bool	isDirective(const Token &token) const;
		const std::set<std::string>	*searchDirectivesSet(const CONTEXT context) const;
		void	printError(const std::string &err_msg, const Token &token) const;
		size_t	countArgs(const TK_TYPE terminating_token) const;
		bool	parseHttpServerEvents();
		bool	parseLocation();
		bool	parseLimitExcept();
		bool	parseAccessLog();
		REQUEST_METHOD	convertToRequestMethod(const std::string &method) const;
		std::string	toUpper(std::string) const;
		Parser();
		void	operator=(const Parser &other);
	public:
		Parser(const std::vector<Token> &tokens, const std::string &filepath);
		~Parser();
		static std::map<std::string, unsigned int>	all_directives_; // すべてのcontextとdirectiveの情報を管理
 		bool	parse();
		const Main	&getConfig() const;
};
} // namespace config

#endif