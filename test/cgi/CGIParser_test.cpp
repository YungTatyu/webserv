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

TEST(cgi_executor, status_error1)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_FALSE(parser.parse(response, "status: OK\r\nstatus: 200 OK\r\n\r\n"));
}

TEST(cgi_executor, status_error2)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	/**
	 * status codeは100以上である必要がある
	 */
	for (int i = -100; i < 100; i++)
	{
		EXPECT_FALSE(parser.parse(response, "status: OK\r\nstatus: 200 OK\r\n\r\n"));
	}
}

TEST(cgi_executor, status_ok1)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nstatus: 200 OK\r\n\r\n"));
	test::expectStatusLine(response, {0, "200 OK"});
}

TEST(cgi_executor, status_ok2)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_TRUE(parser.parse(response, "status:900    \r\nstatus: OK\r\n\r\n"));
	test::expectStatusLine(response, {900, ""});
}

TEST(cgi_executor, status_ok3)
{
	HttpResponse	response;
	cgi::CGIParser	parser(response);

	EXPECT_TRUE(parser.parse(response, "status:    1000000000000000000000000000000000000000000000      random    \r\nstatus: OK\r\n\r\n"));
	test::expectStatusLine(response, {0, "1000000000000000000000000000000000000000000000      random"});
}
