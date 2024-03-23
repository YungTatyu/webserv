#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "CGIHandler.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>

namespace test
{
	expectStatusLine(const HttpResponse& response, const std::pair<long, std::string>& expect)
	{
		EXPECT_EQ(response.status_code_, expect.first);
		EXPECT_EQ(response.cgi_status_code_line_, expect.second);
	}
} // namespace test

TEST(cgi_parser, error_no_status_code)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: OK\r\n\r\n"));
}

TEST(cgi_parser, error_invalid_code)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	/**
	 * status codeは100以上である必要がある
	 */
	for (int i = -100; i < 100; i++)
	{
		EXPECT_FALSE(parser.parse(response, std::string("status:") + std::to_string(i) + " OK\r\n\r\n"));
	}
}

TEST(cgi_parser, status_ok1)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nstatus: 200 OK\r\n\r\n"));
	test::expectStatusLine(response, {0, "200 OK"});
}

TEST(cgi_parser, status_ok2)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_TRUE(parser.parse(response, "status:900    \r\nstatus: OK\r\n\r\n"));
	test::expectStatusLine(response, {900, ""});
}

TEST(cgi_parser, status_ok3)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_TRUE(parser.parse(response, "status:    1000000000000000000000000000000000000000000000      random    \r\nstatus: OK\r\n\r\n"));
	test::expectStatusLine(response, {0, "1000000000000000000000000000000000000000000000      random"});
}

TEST(cgi_parser, error_cl_no_value)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length\r\n\r\n"));
}

TEST(cgi_parser, error_cl_semicolon)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  \r\n\r\n"));
}

TEST(cgi_parser, error_cl_char)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  a\r\n\r\n"));
}

TEST(cgi_parser, error_cl_invalid_num)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  -1\r\n\r\n"));
	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  0\r\n\r\n"));
}

TEST(cgi_parser, error_cl_too_large_length)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  9223372036854775808\r\n\r\n"));
}
