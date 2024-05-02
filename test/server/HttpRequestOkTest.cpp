#include <gtest/gtest.h>

#include "HttpRequest.hpp"
#include "LimitExcept.hpp"

void checkHttpRequestEqual(HttpRequest expect, HttpRequest test) {
  EXPECT_EQ(expect.method, test.method);
  EXPECT_EQ(expect.uri, test.uri);
  EXPECT_EQ(expect.version, test.version);
  EXPECT_EQ(expect.headers, test.headers);
  EXPECT_EQ(expect.queries, test.queries);
  EXPECT_EQ(expect.body, test.body);

  EXPECT_EQ(expect.parseState, test.parseState);
}

TEST(HttpRequest, OkTest1) {
  // testcase: リクエストラインだけ -> Hostヘッダーは無いとエラーなので追加.
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest2) {
  // testcase: header fieldが一対ある時
  // testcase: bodyもあるけど、content-length, Transfer-Encodingどちらもないのでbody無視される
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n"
      "this is body";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest3) {
  // testcase: header fieldが複数ある時
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  headers["name1"] = "value1";
  headers["name2"] = "value2";
  HttpRequest expect(config::GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host:aa\r\n"
      "name1:value1\r\n"
      "name2:value2\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest4) {
  // testcase: query stringが単体
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "query1=value1", "",
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET /html?query1=value1 HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest5) {
  // testcase: query stringが複数ある時
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "query1=value1&query2=value2", "",
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET /html?query1=value1&query2=value2 HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest6) {
  // testcase: chunked first
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  headers["Transfer-Encoding"] = "chunked";
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "hello"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest7) {
  // testcase: chunked first
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  headers["Transfer-Encoding"] = "chunked";
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "hello"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  // testcase: chunked second
  HttpRequest expect2(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked =
      "6\r\n"
      " world"
      "\r\n";
  HttpRequest::parseRequest(chunked, test);

  checkHttpRequestEqual(expect2, test);
}

TEST(HttpRequest, OkTest8) {
  // testcase: chunked first
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  headers["Transfer-Encoding"] = "chunked";
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "hello"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);

  // testcase: chunked second
  HttpRequest expect2(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked =
      "6\r\n"
      " world"
      "\r\n";
  HttpRequest::parseRequest(chunked, test);
  checkHttpRequestEqual(expect2, test);

  // testcase: chunked third (end)
  HttpRequest expect3(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_COMPLETE);

  // test
  std::string chunked2 =
      "0\r\n"
      "\r\n";
  HttpRequest::parseRequest(chunked2, test);

  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, OkTest9) {
  // testcase: encoded url
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::GET, "/Hello World!", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET /Hello%20World%21 HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest10) {
  // testcase: POSTをパースできるか
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::POST, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "POST / HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest11) {
  // testcase: HEADをパースできるか.
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::HEAD, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "HEAD / HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest12) {
  // testcase: HEADをパースできるか.
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::HEAD, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "HEAD / HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest13) {
  // testcase: DELETEをパースできるか.
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers;
  headers["Host"] = "aa";
  HttpRequest expect(config::DELETE, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "DELETE / HTTP/1.1\r\n"
      "Host: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest14) {
  // testcase: header fieldが一対ある時
  // testcase: bodyある
  std::string body = "this is body";
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}, {"Content-Length", std::to_string(body.size())}}, "", body, HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      std::string("GET / HTTP/1.1\r\n")
      + "Host: aa\r\n"
      + "Content-Length: " + std::to_string(body.size()) + "\r\n"
      + "\r\n"
      + body;
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest15) {
  // testcase: body with next request
  std::string body = "this is body";
  std::string next_req = "next request";
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}, {"Content-Length", std::to_string(body.size())}}, "", body, HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      std::string("GET / HTTP/1.1\r\n")
      + "Host: aa\r\n"
      + "Content-Length: " + std::to_string(body.size()) + "\r\n"
      + "\r\n"
      + body
      + next_req;
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
  EXPECT_EQ(rawRequest, next_req); // 次のリクエストは残ったまま
}

TEST(HttpRequest, OkTest16) {
  // testcase: header headerにコロンがない
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}, {"test", ""}}, "", "", HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      std::string("GET / HTTP/1.1\r\n")
      + "Host: aa\r\n"
      + "test\r\n"
      + "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(HttpRequest, OkTest17) {
  // testcase: chunked
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "hello"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);

  // testcase: chunked second
  HttpRequest expect2(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked =
      "6\r\n"
      " world"
      "\r\n";
  HttpRequest::parseRequest(chunked, test);
  checkHttpRequestEqual(expect2, test);

  // testcase: chunked third
  HttpRequest expect3(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked2 = "0\r\n";
  HttpRequest::parseRequest(chunked2, test);

  checkHttpRequestEqual(expect3, test);


  // testcase: chunked forth (end)
  HttpRequest expect4(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_COMPLETE);

  // test
  std::string chunked3 = "\r\n";
  HttpRequest::parseRequest(chunked3, test);

  checkHttpRequestEqual(expect4, test);
}

TEST(HttpRequest, OkTest18) {
  // testcase: chunked with \r\n and bytes
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "0\r\n\r\n", HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "0\r\n\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);

  // testcase: chunked second
  HttpRequest expect2(config::GET, "/html", "HTTP/1.1", headers, "", "0\r\n\r\n\r\n9\r\n\r",
                      HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked =
      "6\r\n"
      "\r\n9\r\n\r"
      "\r\n";
  HttpRequest::parseRequest(chunked, test);
  checkHttpRequestEqual(expect2, test);

  // testcase: chunked third
  HttpRequest expect3(config::GET, "/html", "HTTP/1.1", headers, "", "0\r\n\r\n\r\n9\r\n\r",
                      HttpRequest::PARSE_COMPLETE);

  // test
  std::string chunked2 = "0\r\n\r\n";
  HttpRequest::parseRequest(chunked2, test);

  checkHttpRequestEqual(expect3, test);
}
