#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

/*
 *  root + try_files in location
*/
TEST(HttpResponseRoot, current_directory) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

TEST(HttpResponseRoot, current_directory_slash) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/a/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

TEST(HttpResponseRoot, parent_directory) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/b/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
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

TEST(HttpResponseRoot, parent_directory_slash) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/c/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
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

TEST(HttpResponseRoot, current_directory_alias) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/d/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

/*
 *  root + index in location
*/
TEST(HttpResponseRoot, current_directory_index) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4222}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

TEST(HttpResponseRoot, current_directory_slash_index) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4233}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

TEST(HttpResponseRoot, parent_directory_index) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
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

TEST(HttpResponseRoot, parent_directory_slash_index) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4255}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
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

TEST(HttpResponseRoot, current_directory_index_alias) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4266}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(403);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testBody(expect_body);
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}

/*
 * no location
 */
TEST(HttpResponseRoot, current_directory_no_location) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4222}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

TEST(HttpResponseRoot, current_directory_slash_no_location) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4233}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
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

TEST(HttpResponseRoot, parent_directory_no_location) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
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

TEST(HttpResponseRoot, parent_directory_slash_no_location) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4255}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
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
