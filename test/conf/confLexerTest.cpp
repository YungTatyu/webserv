#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Lexer.hpp"
#include "conf.hpp"

namespace test {
void expectSameToken(const config::TK_TYPE type, const std::string& value, const unsigned int line,
                     const struct config::Token& token) {
  EXPECT_EQ(value, token.value_);
  EXPECT_EQ(type, token.type_);
  EXPECT_EQ(line, token.line_);
}

const config::Token& getToken(const std::vector<config::Token>& tokens, int key) { return tokens[key]; }

}  // namespace test

TEST(ConfigTest, no_file) {
  std::string filePath = "";

  EXPECT_FALSE(config::initConfig(filePath));
}

TEST(ConfigTest, is_dir) {
  std::string filePath = "test/conf/LexerTestFiles/dir.conf";

  EXPECT_FALSE(config::initConfig(filePath));
}
/*
TEST(ConfigTest, inaccessible_file)
{
        std::string filePath = "test/conf/LexerTestFiles/inaccessible.conf";

        EXPECT_FALSE(config::initConfig(filePath));
}
*/
TEST(LexerTokenizeTest, empty_file) {
  std::string filePath = "test/conf/LexerTestFiles/empty.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_END, "", 1, test::getToken(lexer.getTokens(), 0));
}

TEST(LexerTokenizeTest, one_directive) {
  std::string filePath = "test/conf/LexerTestFiles/lexer1.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "error_log", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_STR, "webserv/logs", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 1, test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 1, test::getToken(lexer.getTokens(), 3));
}

TEST(LexerTokenizeTest, events_context) {
  std::string filePath = "test/conf/LexerTestFiles/lexer2.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_STR, "worker_connections", 2,
                        test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "1024", 2, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 2, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_STR, "use", 3, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_STR, "poll", 3, test::getToken(lexer.getTokens(), 6));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 3, test::getToken(lexer.getTokens(), 7));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 4, test::getToken(lexer.getTokens(), 8));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 4, test::getToken(lexer.getTokens(), 9));
}

TEST(LexerTokenizeTest, http_context) {
  std::string filePath = "test/conf/LexerTestFiles/lexer3.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 2, test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "http", 4, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 4, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_STR, "access_log", 5, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_STR, "logs/access.log", 5, test::getToken(lexer.getTokens(), 6));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 5, test::getToken(lexer.getTokens(), 7));
  test::expectSameToken(config::TK_TYPE::TK_STR, "server", 7, test::getToken(lexer.getTokens(), 8));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 7, test::getToken(lexer.getTokens(), 9));
  test::expectSameToken(config::TK_TYPE::TK_STR, "root", 8, test::getToken(lexer.getTokens(), 10));
  test::expectSameToken(config::TK_TYPE::TK_STR, "/home/student/webserv/html", 8,
                        test::getToken(lexer.getTokens(), 11));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 8, test::getToken(lexer.getTokens(), 12));
  test::expectSameToken(config::TK_TYPE::TK_STR, "listen", 9, test::getToken(lexer.getTokens(), 13));
  test::expectSameToken(config::TK_TYPE::TK_STR, "7000", 9, test::getToken(lexer.getTokens(), 14));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 9, test::getToken(lexer.getTokens(), 15));
  test::expectSameToken(config::TK_TYPE::TK_STR, "location", 11, test::getToken(lexer.getTokens(), 16));
  test::expectSameToken(config::TK_TYPE::TK_STR, "/", 11, test::getToken(lexer.getTokens(), 17));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 11, test::getToken(lexer.getTokens(), 18));
  test::expectSameToken(config::TK_TYPE::TK_STR, "index", 12, test::getToken(lexer.getTokens(), 19));
  test::expectSameToken(config::TK_TYPE::TK_STR, "index.html", 12, test::getToken(lexer.getTokens(), 20));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 12, test::getToken(lexer.getTokens(), 21));
  test::expectSameToken(config::TK_TYPE::TK_STR, "allow", 13, test::getToken(lexer.getTokens(), 22));
  test::expectSameToken(config::TK_TYPE::TK_STR, "192.0.0.1/24", 13, test::getToken(lexer.getTokens(), 23));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 13, test::getToken(lexer.getTokens(), 24));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 14,
                        test::getToken(lexer.getTokens(), 25));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 15,
                        test::getToken(lexer.getTokens(), 26));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 16,
                        test::getToken(lexer.getTokens(), 27));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 16, test::getToken(lexer.getTokens(), 28));
}

TEST(LexerTokenizeTest, comment_skip) {
  std::string filePath = "test/conf/LexerTestFiles/comment.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_STR, "worker_connections", 2,
                        test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "1024", 2, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 2, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 4, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_STR, "http", 6, test::getToken(lexer.getTokens(), 6));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 6, test::getToken(lexer.getTokens(), 7));
  test::expectSameToken(config::TK_TYPE::TK_STR, "error_log", 7, test::getToken(lexer.getTokens(), 8));
  test::expectSameToken(config::TK_TYPE::TK_STR, "/home/students/webserv/logs/error.log", 7,
                        test::getToken(lexer.getTokens(), 9));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 7, test::getToken(lexer.getTokens(), 10));
  test::expectSameToken(config::TK_TYPE::TK_STR, "server", 9, test::getToken(lexer.getTokens(), 11));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 9, test::getToken(lexer.getTokens(), 12));
  test::expectSameToken(config::TK_TYPE::TK_STR, "listen", 11, test::getToken(lexer.getTokens(), 13));
  test::expectSameToken(config::TK_TYPE::TK_STR, "0.0.0.0:3001", 11, test::getToken(lexer.getTokens(), 14));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 11, test::getToken(lexer.getTokens(), 15));
  test::expectSameToken(config::TK_TYPE::TK_STR, "root", 12, test::getToken(lexer.getTokens(), 16));
  test::expectSameToken(config::TK_TYPE::TK_STR, "/home/student/webserv/html", 12,
                        test::getToken(lexer.getTokens(), 17));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 12, test::getToken(lexer.getTokens(), 18));
  test::expectSameToken(config::TK_TYPE::TK_STR, "index", 13, test::getToken(lexer.getTokens(), 19));
  test::expectSameToken(config::TK_TYPE::TK_STR, "index.html", 13, test::getToken(lexer.getTokens(), 20));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 13, test::getToken(lexer.getTokens(), 21));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 14,
                        test::getToken(lexer.getTokens(), 22));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 15,
                        test::getToken(lexer.getTokens(), 23));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 15, test::getToken(lexer.getTokens(), 24));
}

TEST(LexerTokenizeTest, only_comment) {
  std::string filePath = "test/conf/LexerTestFiles/only_comment.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_END, "", 82, test::getToken(lexer.getTokens(), 0));
}

TEST(LexerTokenizeTest, quote_file) {
  std::string filePath = "test/conf/LexerTestFiles/quote.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_STR, "worker_connections", 2,
                        test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "1024", 2, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_STR, ";", 2, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_STR, "use", 3, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_STR, "select", 3, test::getToken(lexer.getTokens(), 6));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 3, test::getToken(lexer.getTokens(), 7));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 4, test::getToken(lexer.getTokens(), 8));
  test::expectSameToken(config::TK_TYPE::TK_STR, "http", 6, test::getToken(lexer.getTokens(), 9));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 6, test::getToken(lexer.getTokens(), 10));
  test::expectSameToken(config::TK_TYPE::TK_STR, "error_log", 7, test::getToken(lexer.getTokens(), 11));
  test::expectSameToken(config::TK_TYPE::TK_STR, "/home/students/webserv/logs/error.log", 8,
                        test::getToken(lexer.getTokens(), 12));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 8, test::getToken(lexer.getTokens(), 13));
  test::expectSameToken(config::TK_TYPE::TK_STR, "server", 10, test::getToken(lexer.getTokens(), 14));
  test::expectSameToken(config::TK_TYPE::TK_STR, "{", 10, test::getToken(lexer.getTokens(), 15));
  test::expectSameToken(config::TK_TYPE::TK_STR, "listen", 11, test::getToken(lexer.getTokens(), 16));
  test::expectSameToken(config::TK_TYPE::TK_STR, "0.0.0.0:3001", 11, test::getToken(lexer.getTokens(), 17));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 11, test::getToken(lexer.getTokens(), 18));
  test::expectSameToken(config::TK_TYPE::TK_STR, "root", 12, test::getToken(lexer.getTokens(), 19));
  test::expectSameToken(config::TK_TYPE::TK_STR, "/home/student/webserv/html", 12,
                        test::getToken(lexer.getTokens(), 20));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 12, test::getToken(lexer.getTokens(), 21));
  test::expectSameToken(config::TK_TYPE::TK_STR, "index index.html;\n\t}\n}", 15,
                        test::getToken(lexer.getTokens(), 22));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 15, test::getToken(lexer.getTokens(), 23));
}

TEST(LexerTokenizeTest, continuous_quote) {
  std::string filePath = "test/conf/LexerTestFiles/continuous_quote.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_STR, "worker_connections", 2,
                        test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "", 2, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_STR, "1024", 2, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 2, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 3, test::getToken(lexer.getTokens(), 6));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 4, test::getToken(lexer.getTokens(), 7));
}

TEST(LexerTokenizeTest, single_in_double_quote) {
  std::string filePath = "test/conf/LexerTestFiles/single_in_double_quote.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_STR, "''", 2, test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "1024", 2, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 2, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 3, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 3, test::getToken(lexer.getTokens(), 6));
}

TEST(LexerTokenizeTest, double_in_single_quote) {
  std::string filePath = "test/conf/LexerTestFiles/double_in_single_quote.conf";
  config::Lexer lexer(filePath);

  lexer.tokenize();

  test::expectSameToken(config::TK_TYPE::TK_STR, "events", 1, test::getToken(lexer.getTokens(), 0));
  test::expectSameToken(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, test::getToken(lexer.getTokens(), 1));
  test::expectSameToken(config::TK_TYPE::TK_STR, "\"\"", 2, test::getToken(lexer.getTokens(), 2));
  test::expectSameToken(config::TK_TYPE::TK_STR, "1024", 2, test::getToken(lexer.getTokens(), 3));
  test::expectSameToken(config::TK_TYPE::TK_SEMICOLON, ";", 2, test::getToken(lexer.getTokens(), 4));
  test::expectSameToken(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 3, test::getToken(lexer.getTokens(), 5));
  test::expectSameToken(config::TK_TYPE::TK_END, "", 3, test::getToken(lexer.getTokens(), 6));
}
