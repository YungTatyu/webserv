#include "Lexer.hpp"
#include <iostream>
#include <fstream>

config::Lexer::Lexer(const std::string file_path)
	: file_content_(getFileContent(file_path)), file_iterator_(0), current_line_(1)
{
	#ifdef Debug
	std::cout << "Lexer Constructor" << std::endl;
	#endif
}

config::Lexer::~Lexer()
{
	#ifdef Debug
	std::cout << "Lexer Destructor" << std::endl;
	#endif
}

void	config::Lexer::tokenize()
{
	unsigned int	current_line = 1;

	while (!isEndOfFile()) {
		skipSpaces();
		addToken();
	}
}

const config::Token&	config::Lexer::getToken(int key)
{
	return this->tokens_[key];
}

std::string	config::Lexer::getFileContent(const std::string file_path)
{
    // ファイルを開く
    std::ifstream file(file_path);

    // ファイルが開けたか確認
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << file_path << std::endl;
        return ""; // 後で例外投げる
    }

    // ファイルから読み取ったデータを格納するための変数
    std::string content;
    std::string line;

    // ファイルから1行ずつ読み取り、contentに追加
    while (std::getline(file, line)) {
        content += line + "\n"; // 改行を保持する場合
        // または content += line; で改行を保持せずに追加することも可能
    }

    // ファイルを閉じる
    file.close();

    // ファイルから読み取った内容を出力
    std::cout << "File Content:\n" << content << std::endl;

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

const char&	config::Lexer::getChar()
{
	return file_content_[file_iterator_];
}

bool	config::Lexer::isMetaChar()
{
	if (std::isspace(getChar()) \
		|| getChar() == '{' \
		|| getChar() == '}' \
		|| getChar() == ';')
		return true;
	return false;
}

void	config::Lexer::addToken()
{
	//struct config::Token	new_token;

	if (isEndOfFile())
		return ;

	if (getChar() == '{')
	{
		struct config::Token	new_token(std::string(1, getChar()), config::TK_TYPE::TK_OPEN_CURLY_BRACE, this->current_line_);
		tokens_.push_back(new_token);
		/*new_token.value_ += getChar();
		new_token.type_ = TK_OPEN_CURLY_BRACE;*/
		file_iterator_++;
	std::cout << new_token.value_ << new_token.type_ << new_token.line_ << std::endl;
	}
	else if (getChar() == '}')
	{
		struct config::Token	new_token(std::string(1, getChar()), config::TK_TYPE::TK_CLOSE_CURLY_BRACE, this->current_line_);
		tokens_.push_back(new_token);
		/*
		new_token.value_ += getChar();
		new_token.type_ = TK_CLOSE_CURLY_BRACE;*/
		file_iterator_++;
	std::cout << new_token.value_ << new_token.type_ << new_token.line_ << std::endl;
	}
	else if (getChar() == ';')
	{
		struct config::Token	new_token(std::string(1, getChar()), config::TK_TYPE::TK_SEMICOLON, this->current_line_);
		tokens_.push_back(new_token);
		/*
		new_token.value_ += getChar();
		new_token.type_ = TK_SEMICOLON;*/
		file_iterator_++;
	std::cout << new_token.value_ << new_token.type_ << new_token.line_ << std::endl;
	}
	else
	{
		std::string	tmp_value;
		while (!isEndOfFile() && !isMetaChar())
		{
			tmp_value += getChar();
			file_iterator_++;
		}
		struct config::Token	new_token(tmp_value, config::TK_TYPE::TK_STR, this->current_line_);
		tokens_.push_back(new_token);
	std::cout << new_token.value_ << new_token.type_ << new_token.line_ << std::endl;
	}

	//tokens_.push_back(new_token);
}

bool	config::Lexer::isEndOfFile()
{
	if (file_iterator_ == file_content_.size())
		return true;
	return false;
}

