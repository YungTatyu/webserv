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
		const std::vector<Token>	&tokens_;
		std::set<std::string>	set_contexts_;
		const std::string	filepath_;
		size_t	ti; // token index
		CONTEXT	current_context_;
		static std::map<std::string, unsigned int>	directive_type_;
		Main	config_;
		bool	parseType(const std::string &directive);
		bool	expect(const config::TK_TYPE type);
		bool	isContext(const Token &token);
		bool	isDirective(const Token &token);
		void	printError(const std::string &err_msg) const;
		bool	parseAccessLog();
		std::map<std::string, bool (config::Parser::*)()>	directives_parser_map_;
		Parser();
	public:
		Parser(const std::vector<Token> &tokens, const std::string &filepath);
		~Parser();
		static std::map<std::string, unsigned int>	all_contexts_;
		static std::map<std::string, unsigned int>	all_directives_;
		// static std::set<std::string>	all_contexts_;
		// static std::set<std::string>	all_directives_;
		bool	parse();
};
} // namespace config

#endif