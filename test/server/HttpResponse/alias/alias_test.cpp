#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseAlias, location_index) {
  test::ResponseTest test("test/server/HttpResponse/alias/file/alias.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/alias1/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = utils::readFile("test/server/HttpResponse/alias/alias_dir/alias1.html");
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

TEST(HttpResponseAlias, server_index) {
  test::ResponseTest test("test/server/HttpResponse/alias/file/alias.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/alias2/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = utils::readFile("test/server/HttpResponse/alias/alias_dir/server1.html");
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

TEST(HttpResponseAlias, location_index_dir_listing) {
  test::ResponseTest test("test/server/HttpResponse/alias/file/alias.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/alias3/", HttpRequest::PARSE_COMPLETE));

  std::string dir_path = test.getAbsolutePath("test/server/HttpResponse/alias/alias_dir/");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", "555"},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
  test.testAutoindexEntry(test.responses_[0].body_, 3);
  test.testAutoindexEntry(test.responses_[1].body_, 3);
}

TEST(HttpResponseAlias, server_index_dir_listing) {
  test::ResponseTest test("test/server/HttpResponse/alias/file/alias.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/alias4/", HttpRequest::PARSE_COMPLETE));

  std::string dir_path = test.getAbsolutePath("test/server/HttpResponse/alias/alias_dir/");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", "555"},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
  test.testAutoindexEntry(test.responses_[0].body_, 3);
  test.testAutoindexEntry(test.responses_[1].body_, 3);
}

TEST(HttpResponseAlias, http_index_dir_listing) {
  test::ResponseTest test("test/server/HttpResponse/alias/file/alias.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4246}, {"127.0.0.1", 4247}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/alias5/", HttpRequest::PARSE_COMPLETE));

  std::string dir_path = test.getAbsolutePath("test/server/HttpResponse/alias/alias_dir/");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", "555"},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
  test.testAutoindexEntry(test.responses_[0].body_, 3);
  test.testAutoindexEntry(test.responses_[1].body_, 3);
}
