#include "Lexer.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

config::Lexer::Lexer(const std::string& file_path)
    : file_content_(getFileContent(file_path)), file_iterator_(0), current_line_(1) {}

void config::Lexer::tokenize() {
  while (!isEndOfFile()) {
    skipSpaces();
    skipComment();
    addToken();
  }

  config::Token end_token("", config::TK_END, this->current_line_);
  this->tokens_.push_back(end_token);
}

const std::vector<config::Token>& config::Lexer::getTokens() const { return this->tokens_; }

const std::string config::Lexer::getFileContent(const std::string file_path) const {
  // ファイルを開く
  std::ifstream file(file_path.c_str());

  // ファイルから読み取ったデータを格納するための変数
  std::string content;
  std::string line;

  // ファイルから1行ずつ読み取り、contentに追加
  while (std::getline(file, line)) {
    content += line;

    // 次の行があるときにだけ改行を追加する
    if (file.peek() != EOF) content += "\n";
  }

  // ファイルを閉じる
  file.close();

// ファイルから読み取った内容を出力
#ifdef TEST
  std::cout << "File Content:" << std::endl << content << std::endl;
#endif

  return content;
}

void config::Lexer::skipSpaces() {
  if (isEndOfFile()) return;
  while (!isEndOfFile() && std::isspace(getChar())) {
    if (getChar() == '\n') current_line_++;
    file_iterator_++;
  }
}

void config::Lexer::skipComment() {
  if (isEndOfFile()) return;

  if (getChar() == '#') {
    file_iterator_++;
    while (!isEndOfFile() && getChar() != '\n') {
      file_iterator_++;
    }
    if (!isEndOfFile()) {
      file_iterator_++;
      current_line_++;
      skipSpaces();
    }
  }
}

const char& config::Lexer::getChar() const { return file_content_[file_iterator_]; }

bool config::Lexer::isMetaChar() const {
  if (std::isspace(getChar()) || getChar() == '{' || getChar() == '}' || getChar() == ';') return true;
  return false;
}

void config::Lexer::addToken() {
  if (isEndOfFile() || getChar() == '#') return;

  std::string tmp_value;
  TK_TYPE tmp_type;
  unsigned int tmp_line = this->current_line_;
  char tmp_quote;

  switch (getChar()) {
    case '{':
      tmp_value += getChar();
      tmp_type = config::TK_OPEN_CURLY_BRACE;
      file_iterator_++;
      break;
    case '}':
      tmp_value += getChar();
      tmp_type = config::TK_CLOSE_CURLY_BRACE;
      file_iterator_++;
      break;
    case ';':
      tmp_value += getChar();
      tmp_type = config::TK_SEMICOLON;
      file_iterator_++;
      break;
    case '\'':
    case '"':
      tmp_quote = getChar();
      tmp_type = config::TK_STR;
      file_iterator_++;
      while (!isEndOfFile() && getChar() != tmp_quote) {
        if (getChar() == '\n') this->current_line_++;
        tmp_value += getChar();
        file_iterator_++;
      }
      if (getChar() == tmp_quote) file_iterator_++;
      tmp_line = this->current_line_;
      break;
    default:
      while (!isEndOfFile() && !isMetaChar()) {
        tmp_value += getChar();
        file_iterator_++;
      }
      tmp_type = config::TK_STR;
      break;
  }

  config::Token new_token(tmp_value, tmp_type, tmp_line);
  tokens_.push_back(new_token);
}

bool config::Lexer::isEndOfFile() const {
  if (file_iterator_ == file_content_.size()) return true;
  return false;
}
