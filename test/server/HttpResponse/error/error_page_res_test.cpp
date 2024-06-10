#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "LimitExcept.hpp"
#include "ResponseTest.hpp"

TEST(HttpResponseError, not_slash_ending_dir) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4246}, {"127.0.0.1", 4247}});
  test.initRequest({{"host", "test"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::GET}, "/file",
                   HttpRequest::PARSE_COMPLETE);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(301))},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testBody(test.createDefaultErrorBody(301));
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[301]));
}

TEST(HttpResponseError, not_found) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "test"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::GET},
                   "/nothing/", HttpRequest::PARSE_COMPLETE);
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
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::GET}, "/",
                   HttpRequest::PARSE_ERROR);
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
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::GET},
                   "/forbidden/", HttpRequest::PARSE_COMPLETE);
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

TEST(HttpResponseError, not_allowed) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}},
                   {config::REQUEST_METHOD::POST, config::REQUEST_METHOD::DELETE}, "/",
                   HttpRequest::PARSE_COMPLETE);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(405))},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testBody(test.createDefaultErrorBody(405));
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[405]));
}

TEST(HttpResponseError, not_implemented) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}, {"transfer-encoding", "not implemented"}},
                   {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_NOT_IMPLEMENTED);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(501))},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testBody(test.createDefaultErrorBody(501));
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[501]));
}

/*
 * HEAD method ver.
 */

TEST(HttpResponseError, not_slash_ending_dir_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4246}, {"127.0.0.1", 4247}});
  test.initRequest({{"host", "test"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::HEAD}, "/file",
                   HttpRequest::PARSE_COMPLETE);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(301))},
      {"Content-Type", "text/html"},
      {"Connection", "keep-alive"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[301]));
}

TEST(HttpResponseError, not_found_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "test"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::HEAD},
                   "/nothing/", HttpRequest::PARSE_COMPLETE);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(404))},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[404]));
}

TEST(HttpResponseError, bad_request_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::HEAD}, "/",
                   HttpRequest::PARSE_ERROR);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(400))},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[400]));
}

TEST(HttpResponseError, forbidden_HEAD) {
  test::ResponseTest test("test/server/HttpResponse/error/file/error.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUp());
  test.initTiedServers({{"127.0.0.1", 4242}, {"127.0.0.1", 4243}});
  test.initRequest({{"host", "42"}, {"User-Agent", "Mozilla/5.0"}}, {config::REQUEST_METHOD::HEAD},
                   "/forbidden/", HttpRequest::PARSE_COMPLETE);
  test.generateResponse();

  test.testHeaders({
      {"Server", "webserv/1.0"},
      {"Date", ""},
      {"Content-Length", std::to_string(test.calcDefaultBodySize(403))},
      {"Content-Type", "text/html"},
      {"Connection", "close"},
  });
  test.testResponse(test.createResponse(HttpResponse::status_line_map_[403]));
}
