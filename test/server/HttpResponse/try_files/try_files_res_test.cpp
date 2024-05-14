#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseTry_files, in_server) {
  test::ResponseTest test("test/server/HttpResponse/try_files/file/try_files1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}, {"127.0.0.1", 4245}},
                                        {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/try_files/file/server1.html");
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

TEST(HttpResponseTry_files, in_loc) {
  test::ResponseTest test("test/server/HttpResponse/try_files/file/try_files1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/try_files/file/loc1.html");
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

TEST(HttpResponseTry_files, internal_redirect) {
  test::ResponseTest test("test/server/HttpResponse/try_files/file/try_files_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));
  const std::string expect_body = Utils::readFile("test/server/HttpResponse/try_files/file/loc1.html");
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

TEST(HttpResponseTry_files, internal_redirect_dir_nothing) {
  test::ResponseTest test("test/server/HttpResponse/try_files/file/try_files_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/no-dir/", HttpRequest::PARSE_COMPLETE));
  const std::string expect_body = test.createDefaultErrorBody(500);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[500]));
}

TEST(HttpResponseTry_files, internal_redirect_file_nothing) {
  test::ResponseTest test("test/server/HttpResponse/try_files/file/try_files_redirect.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "mstk"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/no-file/", HttpRequest::PARSE_COMPLETE));
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

TEST(HttpResponseTry_files, error_code) {
  test::ResponseTest test("test/server/HttpResponse/try_files/file/try_files_err.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4244}, {"127.0.0.1", 4245}}, {{"host", "someone"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::POST}, "/", HttpRequest::PARSE_COMPLETE));
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
