#ifndef CONFIG_LEXER_HPP
#define CONFIG_LEXER_HPP

#include <string>
#include <vector>

namespace config
{
struct Token
{
	enum TK_TYPE
	{
		TK_STR,
		TK_NUM,
		TK_OPEN_CURLY_BRACE,
		TK_CLOSE_CURLY_BRACE,
		TK_SEMICOLON,
	};
	const std::string	value_;
	const TK_TYPE	type_;
};

class Lexer
{
	private:
		std::vector<Token>	tokens;
}

} // namespace config


#endif