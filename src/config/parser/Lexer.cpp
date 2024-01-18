#include "Lexer.hpp"
#include <iostream>

Lexer::Lexer(const std::string file_path)
	: content_(readFile(file_path)), file_iterator_(0), current_line_(0)
{
	#ifdef Debug
	std::cout << "Lexer Constructor" << std::endl;
	#endif
}

Lexer::~Lexer()
{
	#ifdef Debug
	std::cout << "Lexer Destructor" << std::endl;
	#endif
}

void	Lexer::tokenize(const std::string &conf_file)
{
	unsigned int	current_line = 1;

	while (!isEndOfFile()) {
		std::cout << *it << " ";
		skipSpaces();
		addToken();
	}
}


std::string	Lexer::readFile(const std::string file_path)
{
    // ファイルを開く
    std::ifstream file(filePath);

    // ファイルが開けたか確認
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filePath << std::endl;
        return 1;
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

void	Lexer::skipSpaces()
{
	if (isEndOfFile())
		return ;
	while (std::isspace(getChar())
	{
		if (getChar() == '\n')
			current_line_++;
		file_iterator_++;
	}
}

const char&	Lexer::getChar()
{
	return file_content_[file_iterator_];
}

bool	Lexer::isMetaChar()
{
	if (std::isspace(getChar()) \
		|| getChar() == '{' \
		|| getChar() == '}' \
		|| getChar() == ';')
		return true;
	return false;
}

void	Lexer::addToken()
{
	std::vector<Token>	new_token;

	if (isEndOfFIle())
		return ;

	if (getChar() == '{' || getChar() == '}' || getChar() == ';')
	{
		new_token.value_ += getChar();
		new_token.type_ = TK_OPEN_CURLY_BRACE;
		file_iterator_++;
	}
	else if (getChar() == '}')
	{
		new_token.value_ += getChar();
		new_token.type_ = TK_CLOSE_CURLY_BRACE;
		file_iterator_++;
	}
	else if (getChar() == ';')
	{
		new_token.value_ += getChar();
		new_token.type_ = TK_SEMICOLON;
		file_iterator_++;
	}
	else
	{
		while (!isEndOfFile() && !isMetaChar())
		{
			new_token.value_ += getChar();
			file_iterator_++;
		}
		new_token.type_ = TK_STR;
	}
	new_token.line_ = current_line_;
	tokens_.push_back(new_token);
}

bool	Lexer::isEndOfFile()
{
	if (file_iterator_ == file_content_.size())
		return true;
	return false;
}



