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
  Token(const Token& other) : value_(other.value_), type_(other.type_), line_(other.line_) {}
  ~Token() {}

 private:
  Token();
  Token& operator=(const Token& other);
};

class Lexer {
 private:
  const std::string file_content_;
  unsigned int file_iterator_;
  unsigned int current_line_;
  std::vector<Token> tokens_;

 public:
  Lexer(const std::string& file_path);
  Lexer(const Lexer& other);
  ~Lexer();
  void tokenize();
  const std::vector<Token>& getTokens() const;

 private:
  Lexer();
  Lexer& operator=(const Lexer& other);
  const std::string getFileContent(const std::string& file_path) const;
  void skipSpaces();
  void skipComment();
  const char& getChar() const;
  bool isMetaChar() const;
  void addToken();
  bool isEndOfFile() const;
};
}  // namespace config

#endif
