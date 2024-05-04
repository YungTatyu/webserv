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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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
      "Host: aa\r\n"
      "\r\n";
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

/* -------------- chunk method test -------------- */

TEST(HttpRequest, chunk_method_get_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "G" "E" "T" " "
  std::string req = "G";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "E";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "T";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::GET, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_get_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "GE" "T "
  std::string req = "GE";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "T ";
  HttpRequest expect2(config::GET, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_post_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "P" "O" "S" "T" " "
  std::string req = "P";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "O";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "S";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "T";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_post_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "PO" "ST" " "
  std::string req = "PO";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "ST";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_delete_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "D" "E" "L" "E" "T" "E" " "
  std::string req = "D";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "E";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "L";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "E";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "T";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "E";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::DELETE, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::DELETE, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_delete_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "DE" "LE" "TE" " "
  std::string req = "DE";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "LE";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "TE";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::DELETE, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::DELETE, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_head_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "H" "E" "A" "D" " "
  std::string req = "H";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "E";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "A";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "D";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::HEAD, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::HEAD, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_method_head_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};
  HttpRequest expect1(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_BEFORE);

  // test: "HE" "AD" " "
  std::string req = "HE";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "AD";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest expect2(config::HEAD, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "/ HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::HEAD, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}
/* -------------- chunk method test end -------------- */

/* -------------- chunk method uri -------------- */

TEST(HttpRequest, chunk_uri_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "/" "u" "r" "i" "/" " "
  HttpRequest expect1(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  std::string req = "POST ";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "u";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "i";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::POST, "/uri/", "", {}, "", "", HttpRequest::PARSE_URI_DONE);
  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/uri/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_uri_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "/u" "ri" "/ "
  HttpRequest expect1(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  std::string req = "POST ";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/u";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "ri";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::POST, "/uri/", "", {}, "", "", HttpRequest::PARSE_URI_DONE);
  req = "/ ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/uri/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_uri_qs_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "/" "?" "?" "?" "t" "=" "l"
  HttpRequest expect1(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  std::string req = "POST /uri";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "?";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "?";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "?";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "t";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "=";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "l";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::POST, "/uri/", "", {}, "??t=l", "", HttpRequest::PARSE_URI_DONE);
  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);


  req =
  "HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/uri/", "HTTP/1.1", headers, "??t=l", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}


TEST(HttpRequest, chunk_uri_qs_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "?u" "=t" "&t" "=l" "ov" "e "
  HttpRequest expect1(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  std::string req = "POST /uri";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "?u";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "=t";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "&t";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "=l";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "ov";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::POST, "/uri", "", {}, "u=t&t=love", "", HttpRequest::PARSE_URI_DONE);
  req = "e ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);


  req =
  "HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/uri", "HTTP/1.1", headers, "u=t&t=love", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_uri_qs_3) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "/u" " "
  HttpRequest expect1(config::POST, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  std::string req = "POST /uri";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/?";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::POST, "/uri/", "", {}, "", "", HttpRequest::PARSE_URI_DONE);
  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "HTTP/1.1\r\n"
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::POST, "/uri/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

/* -------------- chunk method uri end -------------- */

/* -------------- chunk version -------------- */

TEST(HttpRequest, chunk_version_1) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "H" "T" "T" "P" "1" "." "1" "\r" "\n"
  HttpRequest expect1(config::DELETE, "/uri/", "", {}, "t=t", "", HttpRequest::PARSE_URI_DONE);
  std::string req = "DELETE /uri/?t=t ";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "H";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "T";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "T";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "P";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = ".";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::DELETE, "/uri/", "HTTP/1.1", {}, "t=t", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::DELETE, "/uri/", "HTTP/1.1", headers, "t=t", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_version_2) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: "HT" "TP" "/1" ".1" "\r\n" ""
  HttpRequest expect1(config::DELETE, "/uri/", "", {}, "t=t", "", HttpRequest::PARSE_URI_DONE);
  std::string req = "DELETE /uri/?t=t ";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "HT";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "TP";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "/1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = ".1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::DELETE, "/uri/", "HTTP/1.1", {}, "t=t", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req =
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect3(config::DELETE, "/uri/", "HTTP/1.1", headers, "t=t", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_version_3) {
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {{"Host", "aa"}};

  // test: " HTTP/1.1" "\n"
  HttpRequest expect1(config::DELETE, "", "", {}, "", "", HttpRequest::PARSE_METHOD_DONE);
  std::string req = "DELETE /uri/?t=t";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::DELETE, "/uri/", "", {}, "t=t", "", HttpRequest::PARSE_URI_DONE);
  req = " HTTP/1.1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::DELETE, "/uri/", "HTTP/1.1", {}, "t=t", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req =
  "Host: aa\r\n"
  "\r\n";
  HttpRequest expect4(config::DELETE, "/uri/", "HTTP/1.1", headers, "t=t", "", HttpRequest::PARSE_COMPLETE);
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);
}
/* -------------- chunk version end -------------- */

/* -------------- chunk headers -------------- */
TEST(HttpRequest, chunk_headers_1) {
  // test: "H" "o" "s" "t" ":" " " "t" "h" " " " " "\r" "\n" "te" "st" ":te" "st" "\n\r\n"
  HttpRequest expect1(config::HEAD, "/uri/", "HTTP/1.1", {}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  std::string req = "HEAD /uri/? HTTP/1.1\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "H";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "o";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "s";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "t";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = ":";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "t";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "h";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "th"}}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "te";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "st";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = ":te";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "st";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "th"}, {"test", "test"}}, "", "", HttpRequest::PARSE_COMPLETE);
  req = "\n\r\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_headers_2) {
  // test: "Host:" " aa\r\n" "empty1" ":\n" "empty2" "\r\n" "\r" "\n"
  HttpRequest expect1(config::HEAD, "/uri/", "HTTP/1.1", {}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  std::string req = "HEAD /uri/? HTTP/1.1\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "Host:";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = " aa\r\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "empty1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"empty1", ""}}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = ":\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = "empty2";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
  
  HttpRequest expect4(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"empty1", ""}, {"empty2", ""}}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);
 
  HttpRequest expect5(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"empty1", ""}, {"empty2", ""}}, "", "", HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);
}


/* -------------- chunk headers end -------------- */






