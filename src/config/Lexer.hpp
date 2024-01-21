#ifndef CONFIG_LEXER_HPP
#define CONFIG_LEXER_HPP

#include <string>
#include <vector>

namespace config
{
enum TK_TYPE
{
	TK_STR,
	TK_NUM,
	TK_OPEN_CURLY_BRACE,
	TK_CLOSE_CURLY_BRACE,
	TK_SEMICOLON,
};

struct Token
{
	const std::string	value_;
	const TK_TYPE		type_;
	const unsigned int	line_;
};

class Lexer
{
	private:
		std::vector<Token>	tokens;
	public:
		void	tokenize(const std::string &conf_file);
};
} // namespace config


#endif