#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include <vector>
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"


TEST(HttpResponseIndex, index_in_server)
{
	test::ResponseTest test("test/server/HttpResponse/index/file/index1.conf");
	ASSERT_NO_FATAL_FAILURE(
		test.setUpAll(
		{{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
		{{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
		"/",
		HttpRequest::PARSE_COMPLETE
		)
	);

	const std::string expect_body = Utils::readFile("test/server/HttpResponse/index/file/index1.html");
	test.testHeaders({
		{"Server", "webserv/1.0"},
		{"Date", ""},
		{"Content-Length", std::to_string(expect_body.size())},
		{"Content-Type", "text/html"},
		{"Connection", "keep-alive"},
	});
	test.testBody(expect_body);
	test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}