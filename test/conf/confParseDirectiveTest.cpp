#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "conf.hpp"
#include "Parser.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

void	SAME_OBJECT(const config::TK_TYPE type, const std::string value, const unsigned int line, struct config::Token token)
{
	EXPECT_EQ(value, token.value_);
	EXPECT_EQ(type, token.type_);
	EXPECT_EQ(line, token.line_);
}

TEST(ConfigTest, no_file)
{
	std::string filePath = "";

	EXPECT_FALSE(config::initConfig(filePath));
}

TEST(ParserDirectiveTest, AccessLog)
{
	config::Token	dir("access_log", config::TK_STR, 1), arg("logs/access.log", config::TK_STR, 1), semi("", config::TK_SEMICOLON, 1);
	std::vector<config::Token>	tokens_;
	tokens_.push_back(dir);
	tokens_.push_back(arg);
	tokens_.push_back(semi);
	config::Parser	parse(tokens_, "");

	parse.parser_map_["access_log"]();

	EXPECT_EQ(parse.)
}
