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
	const TK_TYPE		type_;
	const unsigned int	line_;
};

class Lexer
{
	private:
		std::string			file_content_;
		unsigned int		file_iterator_;
		unsigned int		current_line_;
		std::vector<Token>	tokens_;
	public:
		Lexer(const std::string file_path);
		void	tokenize();
		FRIEND_TEST(LexerTest, one_directive);
		FRIEND_TEST(LexerPrivateFuncTest, skipSpaces);
		FRIEND_TEST(LexerPrivateFuncTest, getToken);
	private:
		// private functions
		Lexer();
		void	readFile(const std::string file_path);
		void	skipSpaces();
		const char&	getChar();
		bool	isMetaChar();
		void	addToken(); // until space tab newline
		bool	isEndOfFile();
};
} // namespace config


#endif
