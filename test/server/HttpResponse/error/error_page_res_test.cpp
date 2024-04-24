#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseError, not_found) {
  test::ResponseTest test("test/server/HttpResponse/error/file/not_found.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "test"}, {"User-Agent", "Mozilla/5.0"}}, "/", HttpRequest::PARSE_COMPLETE);
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

TEST(HttpResponseError, bad_request) {
  test::ResponseTest test("test/server/HttpResponse/error/file/not_found.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}}, "/", HttpRequest::PARSE_ERROR);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(400))},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testBody(test.createDefaultErrorBody(400));
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[400]));
}

TEST(HttpResponseError, forbidden) {
  test::ResponseTest test("test/server/HttpResponse/error/file/not_found.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}}, "/forbidden/",
                   HttpRequest::PARSE_COMPLETE);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(403))},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testBody(test.createDefaultErrorBody(403));
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}
