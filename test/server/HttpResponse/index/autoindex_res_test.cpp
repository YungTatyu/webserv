#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseAutoindex, in_http_on_and_no_location) {
  test::ResponseTest test("test/server/HttpResponse/index/file/autoindex1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4240}, {"127.0.0.1", 4241}},
                                        {{"host", "_"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  std::string dir_path = test.getAbsolutePath("test/server/HttpResponse/index/file");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(339)},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"}
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
  test.testAutoindexEntry(test.responses_[0].body_, 8);
}

TEST(HttpResponseAutoindex, in_loc_on) {
  test::ResponseTest test("test/server/HttpResponse/index/file/autoindex1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "curl/7.68.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(1070)},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"}
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
  test.testAutoindexEntry(test.responses_[0].body_, 8);
}

TEST(HttpResponseAutoindex, in_server_off) {
  test::ResponseTest test("test/server/HttpResponse/index/file/autoindex1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "curl/7.68.0"}},
      {config::REQUEST_METHOD::GET}, "/error/", HttpRequest::PARSE_COMPLETE));
  std::string expect_body = test.createDefaultErrorBody(403);

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"}
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}

TEST(HttpResponseAutoindex, in_http_on) {
  test::ResponseTest test("test/server/HttpResponse/index/file/autoindex1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "test"}, {"User-Agent", "curl/7.68.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(1070)},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"}
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
  test.testAutoindexEntry(test.responses_[0].body_, 8);
}
