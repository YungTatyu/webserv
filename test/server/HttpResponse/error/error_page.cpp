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
	ASSERT_NO_FATAL_FAILURE(test.setUp());
	test.initConfigHandler({"127.0.0.1", 4242});
	test.initRequest({
		{"host", "test"},
		{"User-Agent", "Mozilla/5.0"}
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
