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
TEST(HttpResponseRoot, root_tryfiles_in_location1) {
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

TEST(HttpResponseRoot, root_tryfiles_in_location2) {
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

TEST(HttpResponseRoot, root_tryfiles_in_location3) {
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

TEST(HttpResponseRoot, root_tryfiles_in_location4) {
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

TEST(HttpResponseRoot, alias_tryfiles_in_location) {
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
 *  root + try_files in location
 *  HEAD method ver.
 */
TEST(HttpResponseRoot, root_tryfiles_in_location1_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_tryfiles_in_location2_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/a/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_tryfiles_in_location3_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/b/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_tryfiles_in_location4_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/c/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, alias_tryfiles_in_location_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test1.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/d/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

/*
 *  root + index in location
 */
TEST(HttpResponseRoot, root_index_in_location1) {
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

TEST(HttpResponseRoot, root_index_in_location2) {
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

TEST(HttpResponseRoot, root_index_in_location3) {
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

TEST(HttpResponseRoot, root_index_in_location4) {
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

TEST(HttpResponseRoot, alias_index_in_location) {
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
 *  root + index in location
 *  HEAD method ver.
 */
TEST(HttpResponseRoot, root_index_in_location1_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4222}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_index_in_location2_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4233}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_index_in_location3_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_index_in_location4_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4255}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, alias_index_in_location_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test2.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4266}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(403);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}

/*
 * no location
 */
TEST(HttpResponseRoot, root_index_no_location1) {
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

TEST(HttpResponseRoot, root_index_no_location2) {
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

TEST(HttpResponseRoot, root_index_no_location3) {
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

TEST(HttpResponseRoot, root_index_no_location4) {
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

/*
 * no location
 * HEAD method ver.
 */
TEST(HttpResponseRoot, root_index_no_location1_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4222}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_index_no_location2_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4233}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_index_no_location3_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4244}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, root_index_no_location4_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/test3.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4255}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/parent.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

/*
 * invalid path
 * .../などは存在しないdir扱い
 */
TEST(HttpResponseRoot, invalid_path1) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4210}, {"127.0.0.1", 4211}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
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

TEST(HttpResponseRoot, invalid_path2) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4220}, {"127.0.0.1", 4221}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
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

TEST(HttpResponseRoot, invalid_path3) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4230}, {"127.0.0.1", 4231}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
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

TEST(HttpResponseRoot, invalid_path4) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4240}, {"127.0.0.1", 4241}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
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

/*
 * invalid path
 * .../などは存在しないdir扱い
 * HEAD method ver.
 */
TEST(HttpResponseRoot, invalid_path1_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4210}, {"127.0.0.1", 4211}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(404);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseRoot, invalid_path2_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4220}, {"127.0.0.1", 4221}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(404);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseRoot, invalid_path3_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4230}, {"127.0.0.1", 4231}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(404);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseRoot, invalid_path4_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4240}, {"127.0.0.1", 4241}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::HEAD}, "/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(404);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

/*
 * path info test
 *
 * cgiファイルへのパスであれば、それ以降をpath_infoとして扱う
 */
TEST(HttpResponseRoot, path_info1) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4240}, {"127.0.0.1", 4241}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD,
                                         config::REQUEST_METHOD::POST, config::REQUEST_METHOD::DELETE},
                                        "/cgi-bin/path_info.php/path/info/", HttpRequest::PARSE_COMPLETE));

  // test member variables
  test.testPathInfo(HttpResponse::RES_EXECUTE_CGI, "/cgi-bin/path_info.php",
                    "/path/info/");
}

TEST(HttpResponseRoot, path_info2) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/cgi-bin/non_exist.php/path/info/", HttpRequest::PARSE_COMPLETE));

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

TEST(HttpResponseRoot, path_info3) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::POST, config::REQUEST_METHOD::DELETE},
      "/cgi-bin/path/info/", HttpRequest::PARSE_COMPLETE));

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

TEST(HttpResponseRoot, path_info4) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4240}, {"127.0.0.1", 4241}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD,
                                         config::REQUEST_METHOD::POST, config::REQUEST_METHOD::DELETE},
                                        "/cgi-bin/path_info.php", HttpRequest::PARSE_COMPLETE));

  // test member variables
  test.testPathInfo(HttpResponse::RES_EXECUTE_CGI, "/cgi-bin/path_info.php",
                    "");
}

TEST(HttpResponseRoot, path_info5) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll({{"127.0.0.1", 4240}, {"127.0.0.1", 4241}},
                                        {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
                                        {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD,
                                         config::REQUEST_METHOD::POST, config::REQUEST_METHOD::DELETE},
                                        "/cgi-bin/path_info.php/path1/path2/", HttpRequest::PARSE_COMPLETE));

  // test member variables
  test.testPathInfo(HttpResponse::RES_EXECUTE_CGI, "/cgi-bin/path_info.php",
                    "/path1/path2/");
}

TEST(HttpResponseRoot, path_info6) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::HEAD, config::REQUEST_METHOD::POST,
       config::REQUEST_METHOD::DELETE},
      "/cgi-bin/path_info.php/a/bc//d//ef/g/h/ij/k/lm/n", HttpRequest::PARSE_COMPLETE));

  // test member variables
  test.testPathInfo(HttpResponse::RES_EXECUTE_CGI, "/cgi-bin/path_info.php",
                    "/a/bc//d//ef/g/h/ij/k/lm/n");
}

TEST(HttpResponseRoot, path_info7) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET, config::REQUEST_METHOD::POST, config::REQUEST_METHOD::DELETE},
      "/cgi-bin/not_executable.py/path/info/", HttpRequest::PARSE_COMPLETE));

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
 * path_info test
 * HEAD method ver.
 */

TEST(HttpResponseRoot, path_info2_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/cgi-bin/non_exist.php/path/info/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = Utils::readFile("test/server/HttpResponse/root/file/current.html");
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[200]));
}

TEST(HttpResponseRoot, path_info3_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/cgi-bin/path/info/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(403);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}

TEST(HttpResponseRoot, path_info7_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/root/file/path_info.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4240}, {"127.0.0.1", 4241}}, {{"host", "test"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::HEAD}, "/cgi-bin/not_executable.py/path/info/", HttpRequest::PARSE_COMPLETE));

  const std::string expect_body = test.createDefaultErrorBody(403);
  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(expect_body.size())},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}
