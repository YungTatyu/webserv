#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include <iostream>
#include <fstream>

void	SAME_TOKEN(const TK_TYPE type, const std::string value, struct Token token)
{
	EXPECT_EQ(type, token.type_);
	EXPECT_EQ(value, token.value_);
}

TEST(LexerPrivateFuncTest, skipSpaces)
{
	Lexer lexer_space();
	Lexer lexer_tab();
	lexer_space.file_iterator_ = 0;
	lexer_tab.file_iterator_ = 0;
	lexer_space.file_content_ = "    abc";
	lexer_tab_.file_content_ = "		abc";

	lexer_space.skipSpaces();
	lexer_tab.skipSpaces();

	EXPECT_EQ("abc", lexer_space.file_content_.substr(file_iterator_);
	EXPECT_EQ("abc", lexer_tab.file_content_.substr(file_iterator_);
}


TEST(LexerPrivateFuncTest, getToken)
{
	Lexer	lexer();
	lexer.file_content_ = "error_log logs/;";
	lexer.file_iterator_ = 0;

	lexer.getToken();

	SAME_TOKEN(TK_STR, "error_log", lexer.tokens_[0]);
}

TEST(LexerTokenizeTest, one_directive)
{
	std::string filePath= "confFIle/lexer1.conf";
	Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(TK_STR, "error_log", lexer.tokens_[0]);
	SAME_TOKEN(TK_STR, "webserv/logs", lexer.tokens_[1]);
	SAME_TOKEN(TK_SEMICOLON, ";", lexer.tokens_[2]);
}


TEST(LexerTokenizeTest, events_context)
{
	std::string filePath= "confFIle/lexer2.conf";
	Lexer	lexer(filePath);

	lexer.tokenize();

	SAME_TOKEN(TK_STR, "events", lexer.tokens_[0]);
	SAME_TOKEN(TK_OPEN_CURLY_BRACE, "{", lexer.tokens_[1]);
	SAME_TOKEN(TK_STR, "worker_connections", lexer.tokens_[2]);
	SAME_TOKEN(TK_STR, "1024", lexer.tokens_[3]);
	SAME_TOKEN(TK_SEMICOLON, ";", lexer.tokens_[4]);
	SAME_TOKEN(TK_STR, "use", lexer.tokens_[5]);
	SAME_TOKEN(TK_STR, "poll", lexer.tokens_[6]);
	SAME_TOKEN(TK_SEMICOLON, ";", lexer.tokens_[7]);
	SAME_TOKEN(TK_CLOSE_CURLY_BRACE, "}", lexer.tokens_[8]);
}

/*
TEST(LexerTokenizeTest, server_context)
{
	std::string filePath= "confFIle/lexer3.conf";
	Lexer	lexer(filePath);

	lexer.tokenize();

 	EXPECT_EQ(TK_STR, lexer.tokens_[0].type_);
 	EXPECT_EQ("error_log", lexer.tokens_[0].value_);
 	EXPECT_EQ(TK_STR, lexer.tokens_[1].type_);
 	EXPECT_EQ("webserv/logs", lexer.tokens_[1].value_);
 	EXPECT_EQ(TK_SEMICOLON, lexer.tokens_[2].type_);
 	EXPECT_EQ(";", lexer.tokens_[2].value_);
}
*/

// main function
int	main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
