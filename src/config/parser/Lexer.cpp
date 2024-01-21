#include "Lexer.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

config::Lexer::Lexer(const std::string& file_path) try
    : file_content_(getFileContent(file_path)), file_iterator_(0), current_line_(1)
{
    // コンストラクタの残りの初期化など
}
catch (const std::runtime_error& e)
{
    // 例外を再throwして呼び出し元でキャッチする
    throw; 
}

void	config::Lexer::tokenize()
{
	unsigned int	current_line = 1;

	while (!isEndOfFile()) {
		skipSpaces();
		skipComment();
		addToken();
	}
}

const config::Token&	config::Lexer::getToken(int key)
{
	return this->tokens_[key];
}

const std::string	config::Lexer::getFileContent(const std::string file_path) const
{
    // ファイルを開く
    std::ifstream file(file_path);

    // ファイルが開けたか確認
    if (!file.is_open()) {
		throw std::runtime_error("Failed to open the file: " + file_path);
    }

    // ファイルから読み取ったデータを格納するための変数
    std::string content;
    std::string line;

    // ファイルから1行ずつ読み取り、contentに追加
    while (std::getline(file, line)) {
        content += line + "\n"; // 改行を保持する場合
    }

    // ファイルを閉じる
    file.close();

    // ファイルから読み取った内容を出力
	#ifdef TEST
    std::cout << "File Content:" << std::endl << content << std::endl;
	#endif

    return content;
}

void	config::Lexer::skipSpaces()
{
	if (isEndOfFile())
		return ;
	while (std::isspace(getChar()))
	{
		if (getChar() == '\n')
			current_line_++;
		file_iterator_++;
	}
}

void	config::Lexer::skipComment()
{
	if (isEndOfFile())
		return ;

	if (getChar() == '#')
	{
		file_iterator_++;
		while (getChar() != '\n')
			file_iterator_++;
		file_iterator_++;
		current_line_++;
		skipSpaces();
	}
}

const char&	config::Lexer::getChar() const
{
	return file_content_[file_iterator_];
}

bool	config::Lexer::isMetaChar() const
{
	if (std::isspace(getChar()) 
		|| getChar() == '{' 
		|| getChar() == '}' 
		|| getChar() == ';')
		return true;
	return false;
}

void	config::Lexer::addToken()
{
	if (isEndOfFile())
		return ;

	std::string		tmp_value;
	TK_TYPE			tmp_type;
	unsigned int	tmp_line = this->current_line_;

	switch (getChar()) {
		case '{':
			tmp_value += getChar();
			tmp_type = config::TK_TYPE::TK_OPEN_CURLY_BRACE;
			file_iterator_++;
			break ;
		case '}':
			tmp_value += getChar();
			tmp_type = config::TK_TYPE::TK_CLOSE_CURLY_BRACE;
			file_iterator_++;
			break ;
		case ';':
			tmp_value += getChar();
			tmp_type = config::TK_TYPE::TK_SEMICOLON;
			file_iterator_++;
			break ;
		default:
			while (!isEndOfFile() && !isMetaChar() && !getChar() != '#')
			{
				tmp_value += getChar();
				file_iterator_++;
			}
			tmp_type = config::TK_TYPE::TK_STR;
			tmp_line = this->current_line_; // くぉーとで囲まれてて行数またがることありそう。
			break ;
	}

	#ifdef TEST
	std::cout << "Token.value_: " << tmp_value << ", Token.type_: " << tmp_type << ", Token.line_: " << tmp_line << std::endl;
	#endif

	config::Token	new_token(tmp_value, tmp_type, tmp_line);
	tokens_.push_back(new_token);
}

bool	config::Lexer::isEndOfFile() const
{
	if (file_iterator_ == file_content_.size())
		return true;
	return false;
}

