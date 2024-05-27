#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseReturn, code) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(404);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseReturn, code_text) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/text/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = "no file";
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/plain"},
      {"Connection", "close"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseReturn, url) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(302);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
      {"Location", "http://localhost:4244/"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[302]));
}

TEST(HttpResponseReturn, code_and_url) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(303);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
      {"Location", "http://localhost:4242/"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[303]));
}

TEST(HttpResponseReturn, non_redirect_code_and_url) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}, {"127.0.0.1", 4245}},
                                        {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET},
                                        "/non-redirect-code/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = "http://localhost:4242/";
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/plain"},
      {"Connection", "keep-alive"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse("306"));
}

/*
 * HEAD method ver.
 */

TEST(HttpResponseReturn, code_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(404);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseReturn, code_text_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/text/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = "no file";
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/plain"},
      {"Connection", "close"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseReturn, url_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(302);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
      {"Location", "http://localhost:4244/"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[302]));
}

TEST(HttpResponseReturn, code_and_url_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(303);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
      {"Location", "http://localhost:4242/"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[303]));
}

TEST(HttpResponseReturn, non_redirect_code_and_url_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/return/file/return_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}, {"127.0.0.1", 4245}},
                                        {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD},
                                        "/non-redirect-code/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = "http://localhost:4242/";
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/plain"},
      {"Connection", "keep-alive"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse("306"));
}
