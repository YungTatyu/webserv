#ifndef CONFIG_LEXER_HPP
#define CONFIG_LEXER_HPP

#include <string>
#include <vector>

namespace config {
enum TK_TYPE {
  TK_STR,
  TK_OPEN_CURLY_BRACE,
  TK_CLOSE_CURLY_BRACE,
  TK_SEMICOLON,
  TK_END
};

struct Token {
  const std::string value_;
  const TK_TYPE type_;
  const unsigned int line_;
  Token(const std::string& value, TK_TYPE type, unsigned int line)
      : value_(value), type_(type), line_(line) {}
};

class Lexer {
 private:
  const std::string file_content_;
  unsigned int file_iterator_;
  unsigned int current_line_;
  std::vector<Token> tokens_;

 public:
  Lexer(const std::string& file_path);
  void tokenize();
  const std::vector<Token>& getTokens() const;

 private:
  // private functions
  Lexer();
  const std::string getFileContent(const std::string file_path) const;
  void skipSpaces();
  void skipComment();
  const char& getChar() const;
  bool isMetaChar() const;
  void addToken();
  bool isEndOfFile() const;
};
}  // namespace config

#endif
