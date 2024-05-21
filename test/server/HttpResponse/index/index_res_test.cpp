#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseIndex, index_in_server) {
  test::ResponseTest test("test/server/HttpResponse/index/file/index1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/index/file/server1.html");
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

TEST(HttpResponseIndex, index_in_loc) {
  test::ResponseTest test("test/server/HttpResponse/index/file/index1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD}, "/error/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/index/file/error/error.html");
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

TEST(HttpResponseIndex, index_default) {
  test::ResponseTest test("test/server/HttpResponse/index/file/index2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "not_exist"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/index/file/index.html");
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

TEST(HttpResponseIndex, no_location_and_not_root_uri) {
  test::ResponseTest test("test/server/HttpResponse/index/file/index2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4246}, {"127.0.0.1", 4247}}, {{"host", "not_exist"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD}, "/file/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/index/file/index.html");
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
