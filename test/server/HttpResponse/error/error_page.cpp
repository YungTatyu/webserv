#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include <vector>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "HttpResponseTest.hpp"


TEST(HttpResponseError, not_found)
{
	test::HttpResponseTest test("test/server/HttpResponse/error/file");
	test.setUp();
	test.initConfigHandler({"127.0.0.1", 4242});
	test.initRequest({
		{"host", "test"},
		{"randome", "randome"}
		},
		"/", HttpRequest::PARSE_COMPLETE
	);
	test.generateResponse();

	test.testBody("");
	test.testResponse("");
}

TEST(HttpResponseError, bad_request)
{

}
