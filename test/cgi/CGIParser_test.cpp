#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "CGIHandler.hpp"
#include "Utils.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>

typedef std::pair<std::string, std::string> string_pair;

namespace test
{
	void	expectStatusLine(const HttpResponse& response, const std::pair<long, std::string>& expect)
	{
		EXPECT_EQ(response.status_code_, expect.first);
		EXPECT_EQ(response.cgi_status_code_line_, expect.second);
	}

	void	expectHeader(const HttpResponse& response, const std::string& expect, const std::string& header, bool found)
	{
		const cgi::string_map_case_insensitive&	headers = response.headers_;
		cgi::string_map_case_insensitive::const_iterator	it = headers.find(header);
		if (!found)
		{
			EXPECT_TRUE(it == headers.end());
			return;
		}
		ASSERT_TRUE(it != headers.end());
		EXPECT_EQ(headers.at(header), expect);
	}

	void	expectHeaders(const HttpResponse& response, const std::vector<string_pair>& test_results)
	{
		const cgi::string_map_case_insensitive& headers = response.headers_;
		// for (std::vector<string_pair>::const_iterator it = test_results.begin();
		// 	it != test_results.end();
		// 	++it
		// )
		// {
		// 	auto hit = headers.find(it->first);
		// 	ASSERT_TRUE(hit != headers.end());
		// 	EXPECT_EQ(*hit, it->second);
		// }
		for (const auto& pair : test_results)
		{
			auto hit = headers.find(pair.first);
			ASSERT_TRUE(hit != headers.end());
			EXPECT_EQ(hit->second, pair.second);
		}
	}
} // namespace test


// ++++++++++++++++++++++++++++++ status code test ++++++++++++++++++++++++++++++
TEST(cgi_parser, error_no_status_code)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_FALSE(parser.parse(response, "status: OK\r\n\r\n", cgi::PARSE_BEFORE));
}

TEST(cgi_parser, error_invalid_code)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	/**
	 * status codeは100以上である必要がある
	 */
	for (int i = -100; i < 100; i++)
	{
		EXPECT_FALSE(parser.parse(response, std::string("status:") + std::to_string(i) + " OK\r\n\r\n", cgi::PARSE_BEFORE));
	}
}

TEST(cgi_parser, status_ok1)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nstatus: 200 OK\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectStatusLine(response, {0, "200 OK"});
}

TEST(cgi_parser, status_ok2)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status:900    \r\nstatus: OK\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectStatusLine(response, {900, ""});
}

TEST(cgi_parser, status_ok3)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status:    1000000000000000000000000000000000000000000000      random    \r\nstatus: OK\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectStatusLine(response, {0, "1000000000000000000000000000000000000000000000      random    "});
}

// ++++++++++++++++++++++++++++++ content-length test ++++++++++++++++++++++++++++++
TEST(cgi_parser, error_cl_no_value)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length\r\n\r\n", cgi::PARSE_BEFORE));
}

TEST(cgi_parser, error_cl_semicolon)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  \r\n\r\n", cgi::PARSE_BEFORE));
}

TEST(cgi_parser, error_cl_char)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  a\r\n\r\n", cgi::PARSE_BEFORE));
}

TEST(cgi_parser, error_cl_invalid_num)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  -1\r\n\r\n", cgi::PARSE_BEFORE));
}

TEST(cgi_parser, error_cl_too_large_length)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_FALSE(parser.parse(response, "status: 200\r\ncontent-length:  9223372036854775808\r\n\r\n", cgi::PARSE_BEFORE));
}

TEST(cgi_parser, cl_ok1)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "Content-Length: 10   \r\nstatus: 200 OK\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "10", "content-length", true);
}

TEST(cgi_parser, cl_ok2)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "Content-Length:9223372036854775807\r\nstatus: 200 OK\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "9223372036854775807", "content-length", true);
}

TEST(cgi_parser, cl_ok3)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "CONTENT-LENGTH:0\r\nstatus: 200 OK\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "0", "content-length", true);
}

TEST(cgi_parser, cl_ok4_duplicate)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200\r\ncontent-length:  9223372036854775807   \r\ncontent-length:1\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "9223372036854775807", "content-length", true);
}

// ++++++++++++++++++++++++++++++ Content-type test ++++++++++++++++++++++++++++++
TEST(cgi_parser, ct_ok1)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\ncontent-type:123 \r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "123 ", "content-Type", true);
}

TEST(cgi_parser, ct_ok2)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type:   text \r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "text ", "content-Type", true);
}

TEST(cgi_parser, ct_ok3)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type:\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "", "CONTENT-TYPE", false);
}

TEST(cgi_parser, ct_ok4)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "", "CONTENT-TYPE", false);
}

TEST(cgi_parser, ct_ok5)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nCONTENT-TYPE:     test/html\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "test/html", "content-type", true);
}

TEST(cgi_parser, ct_ok6)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200\r\ncontent-length: 10\r\ncontent-type:test test  \r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeader(response, "test test  ", "content-Type", true);

}

// ++++++++++++++++++++++++++++++ other fields test ++++++++++++++++++++++++++++++
TEST(cgi_parser, other_ok1)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type: test/html\r\nlocation: \r\ntt\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeaders(response,
	{
		{"Status", "200 OK"},
		{"Content-Type", "test/html"},
		{"Location", ""},
		{"tt", ""}
	});
}

TEST(cgi_parser, other_ok2)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type: test/html\r\nlOcAtIoN\r\ntest:\r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeaders(response,
	{
		{"Status", "200 OK"},
		{"Content-Type", "test/html"},
		{"Location", ""},
		{"test", ""}
	});
}

TEST(cgi_parser, other_ok3)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type: test/html\r\nLocation: /path/to\r\ntest: test \r\n\r\n", cgi::PARSE_BEFORE));
	test::expectHeaders(response,
	{
		{"Status", "200 OK"},
		{"Content-Type", "test/html"},
		{"Location", "/path/to"},
		{"test", "test"}
	});
}

TEST(cgi_parser, other_ok4)
{
	HttpResponse	response;
	cgi::CGIParser	parser;

	EXPECT_TRUE(parser.parse(response, "status: 200 OK\r\nContent-Type: test/html\r\nLocation: /path/to/\ntest: what it  isss   \nyay: , oh yeah   \n\n", cgi::PARSE_BEFORE));
	test::expectHeaders(response,
	{
		{"Status", "200 OK"},
		{"Content-Type", "test/html"},
		{"Location", "/path/to/"},
		{"test", "what it  isss"},
		{"yay", "oh yeah"},
	});
}
