#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "conf.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

void	SAME_TOKEN(const config::TK_TYPE type, const std::string value, const unsigned int line, struct config::Token token)
{
	EXPECT_EQ(value, token.value_);
	EXPECT_EQ(type, token.type_);
	EXPECT_EQ(line, token.line_);
}

TEST(ConfigTest, no_file)
{
	std::string filePath = "";

	EXPECT_FALSE(config::init_config(filePath));
}

TEST(ConfigTest, is_dir)
{
	std::string filePath = "test/conf/confFile/dir.conf";

	EXPECT_FALSE(config::init_config(filePath));
}

TEST(ConfigTest, inaccessible_file)
{
	std::string filePath = "test/conf/confFile/inaccessible.conf";

	EXPECT_FALSE(config::init_config(filePath));
}

TEST(LexerTokenizeTest, empty_file)
{
	std::string filePath= "test/conf/confFile/empty.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_END, "", 1, lexer.getToken(0));
}

TEST(LexerTokenizeTest, one_directive)
{
	std::string filePath= "test/conf/confFile/lexer1.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "error_log", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "webserv/logs", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 1, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 1, lexer.getToken(3));
}


TEST(LexerTokenizeTest, events_context)
{
	std::string filePath= "test/conf/confFile/lexer2.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "worker_connections", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "1024", 2, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 2, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "use", 3, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "poll", 3, lexer.getToken(6));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 3, lexer.getToken(7));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 4, lexer.getToken(8));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 4, lexer.getToken(9));
}

TEST(LexerTokenizeTest, http_context)
{
	std::string filePath= "test/conf/confFile/lexer3.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "http", 4, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 4, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "access_log", 5, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "logs/access.log", 5, lexer.getToken(6));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 5, lexer.getToken(7));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "server", 7, lexer.getToken(8));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 7, lexer.getToken(9));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "root", 8, lexer.getToken(10));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "/home/student/webserv/html", 8, lexer.getToken(11));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 8, lexer.getToken(12));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "listen", 9, lexer.getToken(13));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "7000", 9, lexer.getToken(14));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 9, lexer.getToken(15));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "location", 11, lexer.getToken(16));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "/", 11, lexer.getToken(17));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 11, lexer.getToken(18));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "index", 12, lexer.getToken(19));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "index.html", 12, lexer.getToken(20));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 12, lexer.getToken(21));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "allow", 13, lexer.getToken(22));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "192.0.0.1/24", 13, lexer.getToken(23));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 13, lexer.getToken(24));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 14, lexer.getToken(25));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 15, lexer.getToken(26));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 16, lexer.getToken(27));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 16, lexer.getToken(28));
}

TEST(LexerTokenizeTest, comment_skip)
{
	std::string filePath= "test/conf/confFile/comment.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "worker_connections", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "1024", 2, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 2, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 4, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "http", 6, lexer.getToken(6));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 6, lexer.getToken(7));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "error_log", 7, lexer.getToken(8));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "/home/students/webserv/logs/error.log", 7, lexer.getToken(9));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 7, lexer.getToken(10));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "server", 9, lexer.getToken(11));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 9, lexer.getToken(12));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "listen", 11, lexer.getToken(13));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "0.0.0.0:3001", 11, lexer.getToken(14));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 11, lexer.getToken(15));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "root", 12, lexer.getToken(16));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "/home/student/webserv/html", 12, lexer.getToken(17));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 12, lexer.getToken(18));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "index", 13, lexer.getToken(19));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "index.html", 13, lexer.getToken(20));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 13, lexer.getToken(21));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 14, lexer.getToken(22));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 15, lexer.getToken(23));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 15, lexer.getToken(24));
}

TEST(LexerTokenizeTest, only_comment)
{
	std::string filePath= "test/conf/confFile/only_comment.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_END, "", 82, lexer.getToken(0));
}

TEST(LexerTokenizeTest, quote_file)
{
	std::string filePath= "test/conf/confFile/quote.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "worker_connections", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "1024", 2, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_STR, ";", 2, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "use", 3, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "select", 3, lexer.getToken(6));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 3, lexer.getToken(7));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 4, lexer.getToken(8));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "http", 6, lexer.getToken(9));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 6, lexer.getToken(10));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "error_log", 7, lexer.getToken(11));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "/home/students/webserv/logs/error.log", 8, lexer.getToken(12));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 8, lexer.getToken(13));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "server", 10, lexer.getToken(14));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "{", 10, lexer.getToken(15));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "listen", 11, lexer.getToken(16));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "0.0.0.0:3001", 11, lexer.getToken(17));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 11, lexer.getToken(18));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "root", 12, lexer.getToken(19));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "/home/student/webserv/html", 12, lexer.getToken(20));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 12, lexer.getToken(21));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "index index.html;\n\t}\n}", 13, lexer.getToken(22));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 13, lexer.getToken(23));
}

TEST(LexerTokenizeTest, continuous_quote)
{
	std::string filePath= "test/conf/confFile/continuous_quote.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "worker_connections", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "", 2, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "1024", 2, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 2, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 3, lexer.getToken(6));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 4, lexer.getToken(7));
}

TEST(LexerTokenizeTest, single_in_double_quote)
{
	std::string filePath= "test/conf/confFile/single_in_double_quote.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "''", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "1024", 2, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 2, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 3, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 3, lexer.getToken(6));
}

TEST(LexerTokenizeTest, double_in_single_quote)
{
	std::string filePath= "test/conf/confFile/double_in_single_quote.conf";
	config::Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(config::TK_TYPE::TK_STR, "events", 1, lexer.getToken(0));
	SAME_TOKEN(config::TK_TYPE::TK_OPEN_CURLY_BRACE, "{", 1, lexer.getToken(1));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "\"\"", 2, lexer.getToken(2));
	SAME_TOKEN(config::TK_TYPE::TK_STR, "1024", 2, lexer.getToken(3));
	SAME_TOKEN(config::TK_TYPE::TK_SEMICOLON, ";", 2, lexer.getToken(4));
	SAME_TOKEN(config::TK_TYPE::TK_CLOSE_CURLY_BRACE, "}", 3, lexer.getToken(5));
	SAME_TOKEN(config::TK_TYPE::TK_END, "", 3, lexer.getToken(6));
}

