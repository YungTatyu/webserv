#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include <vector>
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"


TEST(HttpResponseError, not_found)
{
	test::ResponseTest test("test/server/HttpResponse/error/file/not_found.conf");
	ASSERT_NO_FATAL_FAILURE(test.setUp());
	test.initConfigHandler({
		{"127.0.0.1", 4242},
		{"127.0.0.1", 4243}
	});
	test.initRequest({{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
		"/", HttpRequest::PARSE_COMPLETE);
	test.generateResponse();

	test.testHeaders({
		{"Server", "webserv/1.0"},
		{"Date", ""},
		{"Content-Length", std::to_string(test.calcDefaultBodySize(404))},
		{"Content-Type", "text/html"},
		{"Connection", "close"},
	});
	test.testBody(test.createDefaultErrorBody(404));
	test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseError, bad_request)
{

}
