#include <gtest/gtest.h>

#include "HttpRequest.hpp"
#include "LimitExcept.hpp"
#include "cli_max_body_size_test.hpp"

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

  HttpRequest expect3(config::GET, "/html", "HTTP/1.1", headers, "", "hello world",
                      HttpRequest::PARSE_COMPLETE);
  std::string chunked2 =
      "0\n"
      "\n";
  HttpRequest::parseRequest(chunked2, test);
  checkHttpRequestEqual(expect3, test);
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
      "6\n"
      " world"
      "\n";
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
  HttpRequest expect(config::GET, "/", "HTTP/1.1",
                     {{"Host", "aa"}, {"Content-Length", std::to_string(body.size())}}, "", body,
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest = std::string("GET / HTTP/1.1\r\n") + "Host: aa\r\n" +
                           "Content-Length: " + std::to_string(body.size()) + "\r\n" + "\r\n" + body;
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest15) {
  // testcase: body with next request
  std::string body = "this is body";
  std::string next_req = "next request";
  HttpRequest expect(config::GET, "/", "HTTP/1.1",
                     {{"Host", "aa"}, {"Content-Length", std::to_string(body.size())}}, "", body,
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest = std::string("GET / HTTP/1.1\r\n") + "Host: aa\r\n" +
                           "Content-Length: " + std::to_string(body.size()) + "\r\n" + "\r\n" + body +
                           next_req;
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
  EXPECT_EQ(rawRequest, next_req);  // 次のリクエストは残ったまま
}

TEST(HttpRequest, OkTest16) {
  // testcase: header headerにコロンがない
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}, {"test", ""}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest = std::string("GET / HTTP/1.1\r\n") + "Host: aa\r\n" + "test\r\n" + "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest17) {
  // testcase: chunked
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {
      {"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
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
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {
      {"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "0\r\n\r\n",
                     HttpRequest::PARSE_INPROGRESS);

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

TEST(HttpRequest, OkTest19) {
  // testcase: header with space
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", {{"Host", "aa"}, {"space", "this is  test"}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "space:     this is  test     \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest20) {
  // testcase: header with space
  HttpRequest expect(config::GET, "/html", "HTTP/1.1",
                     {{"Host", "aa"}, {"space1", ",   ,,, ,"}, {"space2", ",.     \"\' | |  |   |"}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "space1:     ,   ,,, ,     \r\n"
      "space2:,.     \"\' | |  |   |     \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);
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
  HttpRequest expect3(config::POST, "/uri", "HTTP/1.1", headers, "u=t&t=love", "",
                      HttpRequest::PARSE_COMPLETE);
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

  HttpRequest expect2(config::DELETE, "/uri/", "HTTP/1.1", {}, "t=t", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
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

  HttpRequest expect2(config::DELETE, "/uri/", "HTTP/1.1", {}, "t=t", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
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

  HttpRequest expect3(config::DELETE, "/uri/", "HTTP/1.1", {}, "t=t", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
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

  HttpRequest expect2(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "th"}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
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

  HttpRequest expect3(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "th"}, {"test", "test"}}, "", "",
                      HttpRequest::PARSE_COMPLETE);
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

  HttpRequest expect2(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = " aa\r\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "empty1";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"empty1", ""}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = ":\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = "empty2";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  HttpRequest expect4(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"empty1", ""}, {"empty2", ""}}, "",
                      "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);

  HttpRequest expect5(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"empty1", ""}, {"empty2", ""}}, "",
                      "", HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);
}

TEST(HttpRequest, chunk_headers_3) {
  // test:　duplicate header
  HttpRequest expect1(config::HEAD, "/uri/", "", {}, "", "", HttpRequest::PARSE_URI_DONE);
  std::string req = "HEAD /uri/? HTTP/1.1";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  HttpRequest expect2(config::HEAD, "/uri/", "HTTP/1.1", {}, "", "", HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\nHost";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = ":";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "   aa    ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\ndu";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = "p:";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = " dup_value";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = "  ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = " ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);

  HttpRequest expect4(config::HEAD, "/uri/", "HTTP/1.1", {{"Host", "aa"}, {"dup", "dup_value"}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);

  req = "t";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);

  req = "est:      test ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect4, test);

  HttpRequest expect5(config::HEAD, "/uri/", "HTTP/1.1",
                      {{"Host", "aa"}, {"dup", "dup_value"}, {"test", "test"}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "   \n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);

  req = "dup:   update  ";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);

  req = "\r\nl";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);

  req = "a";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);

  req = "s";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect5, test);

  HttpRequest expect6(config::HEAD, "/uri/", "HTTP/1.1",
                      {{"Host", "aa"}, {"dup", "dup_value"}, {"test", "test"}, {"last", "end"}}, "", "",
                      HttpRequest::PARSE_REQUEST_LINE_DONE);
  req = "t:     end\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect6, test);

  HttpRequest expect7(config::HEAD, "/uri/", "HTTP/1.1",
                      {{"Host", "aa"}, {"dup", "dup_value"}, {"test", "test"}, {"last", "end"}}, "", "",
                      HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect7, test);
}

/* -------------- chunk headers end -------------- */

/* -------------- chunk body -------------- */
TEST(HttpRequest, chunk_chunked_body_1) {
  HttpRequest expect1(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "",
                      HttpRequest::PARSE_INPROGRESS);

  // test:  0 b d 6 \r \n 1 \n \r \t
  std::string req =
      "POST /path/to/uri?user=kh HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "3";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "0";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "b";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "d";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "6";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "1";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\t";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "1";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "e";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "0";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  HttpRequest expect2(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "0bd\r\n1\n\r\te",
                      HttpRequest::PARSE_INPROGRESS);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "0bd\r\n1\n\r\te",
                      HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_chunked_body_2) {
  HttpRequest expect1(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "",
                      HttpRequest::PARSE_INPROGRESS);

  // test:
  std::string req =
      "POST /path/to/uri?user=kh HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  // "   "
  req = "3\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "  ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\ne";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  // this is a body
  req = "th";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "is";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = " i";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "s ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "a ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "bo";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "dy";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\r\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "3\r\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  // hey
  req = "he";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "y\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n0";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  HttpRequest expect2(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "   this is a bodyhey",
                      HttpRequest::PARSE_INPROGRESS);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  HttpRequest expect3(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "   this is a bodyhey",
                      HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_chunked_body_3) {
  HttpRequest expect1(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "",
                      HttpRequest::PARSE_INPROGRESS);

  // test:
  std::string req =
      "POST /path/to/uri?user=kh HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "0";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  HttpRequest expect3(config::POST, "/path/to/uri", "HTTP/1.1",
                      {{"Host", "aa"}, {"transfer-encoding", "chunked"}}, "user=kh", "",
                      HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_content_length_1) {
  HttpRequest expect1(config::POST, "/path/to/uri", "HTTP/1.1", {{"Host", "aa"}, {"content-length", "10"}},
                      "user=kh", "", HttpRequest::PARSE_INPROGRESS);

  // test:
  std::string req =
      "POST /path/to/uri?user=kh HTTP/1.1\r\n"
      "Host: aa\r\n"
      "content-length: 10  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  req = "\r";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "2";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "4";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "\t";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = " ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "h";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "i";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  HttpRequest expect2(config::POST, "/path/to/uri", "HTTP/1.1", {{"Host", "aa"}, {"content-length", "1"}},
                      "user=kh", "\r\n24\n\t hi\n", HttpRequest::PARSE_COMPLETE);
  req = "\n";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);
}

TEST(HttpRequest, chunk_content_length_2) {
  HttpRequest expect1(config::POST, "/path/to/uri", "HTTP/1.1", {{"Host", "aa"}, {"content-length", "14"}},
                      "user=kh", "", HttpRequest::PARSE_INPROGRESS);

  // test: two letters each
  std::string req =
      "POST /path/to/uri?user=kh HTTP/1.1\r\n"
      "Host: aa\r\n"
      "content-length: 14  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);

  // this is a body
  req = "th";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "is";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = " i";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "s ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "a ";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  req = "bo";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parseState);

  HttpRequest expect2(config::POST, "/path/to/uri", "HTTP/1.1", {{"Host", "aa"}, {"content-length", "2"}},
                      "user=kh", "this is a body", HttpRequest::PARSE_COMPLETE);
  req = "dy";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect2, test);

  // next request: bodyに含まれない
  HttpRequest expect3(config::UNKNOWN, "", "", {}, "", "", HttpRequest::PARSE_ERROR);
  req = "over content length";
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_content_length_3) {
  HttpRequest expect1(config::POST, "/path/to/uri", "HTTP/1.1", {{"Host", "aa"}, {"content-length", "0"}},
                      "user=kh", "", HttpRequest::PARSE_COMPLETE);

  // test: content-length 0
  std::string req =
      "POST /path/to/uri?user=kh HTTP/1.1\r\n"
      "Host: aa\r\n"
      "content-length: 0  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect1, test);
}
/* -------------- chunk body end -------------- */

/* -------------- normalize uri -------------- */
TEST(HttpRequest, normalize_uri_1) {
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}}, "", "", HttpRequest::PARSE_COMPLETE);

  // test: multiple slashes
  std::string req =
      "GET /// HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_2) {
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}}, "", "", HttpRequest::PARSE_COMPLETE);

  // test: multiple slashes
  std::string req =
      "GET // HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_3) {
  HttpRequest expect(config::GET, "/path/to/scirpt/", "HTTP/1.1", {{"Host", "aa"}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test: multiple slashes
  std::string req =
      "GET /path/to/scirpt// HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_4) {
  HttpRequest expect(config::GET, "/path/to/scirpt/", "HTTP/1.1", {{"Host", "aa"}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test: multiple slashes
  std::string req =
      "GET /path////to//scirpt/// HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_5) {
  HttpRequest expect(config::GET, "/path/to/scirpt/", "HTTP/1.1", {{"Host", "aa"}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test: multiple slashes
  std::string req =
      "GET //path//to//scirpt// HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_6) {
  HttpRequest expect(config::GET, "/test", "HTTP/1.1", {{"Host", "aa"}}, "", "", HttpRequest::PARSE_COMPLETE);

  // test: dotdot
  std::string req =
      "GET /path/../test HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_7) {
  HttpRequest expect(config::GET, "/test/", "HTTP/1.1", {{"Host", "aa"}}, "", "",
                     HttpRequest::PARSE_COMPLETE);

  // test: dotdot
  std::string req =
      "GET /path/../test/ HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_8) {
  HttpRequest expect(config::GET, "/.../..../path/", "HTTP/1.1", {{"Host", "aa"}}, "query", "",
                     HttpRequest::PARSE_COMPLETE);

  // test: go crazy!
  std::string req =
      "GET //////...///..../path///to//../script///..///?query HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, normalize_uri_9) {
  HttpRequest expect(config::GET, "/", "HTTP/1.1", {{"Host", "aa"}}, "query", "",
                     HttpRequest::PARSE_COMPLETE);

  // test: go crazy!
  std::string req =
      "GET ////.///...///...././//./../////../path//..//to//../script///..?query HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  checkHttpRequestEqual(expect, test);
}

/* -------------- normalize uri end -------------- */

/* -------------- body chunk test -------------- */
TEST(HttpRequest, chunk_hex_1) {
  // testcase: chunked with hex bytes
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {
      {"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "0123456789",
                     HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "a\r\n"
      "0123456789"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);

  // testcase: chunked second
  HttpRequest expect2(config::GET, "/html", "HTTP/1.1", headers, "", "01234567890123456789a",
                      HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked =
      "B\r\n"
      "0123456789a"
      "\r\n";
  HttpRequest::parseRequest(chunked, test);
  checkHttpRequestEqual(expect2, test);

  // testcase: chunked third
  HttpRequest expect3(config::GET, "/html", "HTTP/1.1", headers, "", "01234567890123456789a",
                      HttpRequest::PARSE_COMPLETE);

  // test
  std::string chunked2 = "0\r\n\r\n";
  HttpRequest::parseRequest(chunked2, test);

  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_hex_2) {
  // testcase: chunked with hex bytes
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {
      {"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "", "0123456789abcdef",
                     HttpRequest::PARSE_INPROGRESS);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "10\r\n"  // 16 in decimal
      "0123456789abcdef"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);

  // testcase: chunked second
  HttpRequest expect2(config::GET, "/html", "HTTP/1.1", headers, "",
                      "0123456789abcdef01234567890123456789012345", HttpRequest::PARSE_INPROGRESS);

  // test
  std::string chunked =
      "1A\r\n"  // 26 in decimal
      "01234567890123456789012345"
      "\r\n";
  HttpRequest::parseRequest(chunked, test);
  checkHttpRequestEqual(expect2, test);

  // testcase: chunked third
  HttpRequest expect3(config::GET, "/html", "HTTP/1.1", headers, "",
                      "0123456789abcdef01234567890123456789012345", HttpRequest::PARSE_COMPLETE);

  // test
  std::string chunked2 = "0\r\n\r\n";
  HttpRequest::parseRequest(chunked2, test);

  checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, chunk_hex_3) {
  // testcase: client max body size = 0 means no limits
  test::setupMaxBodySize(0);
  std::map<std::string, std::string, Utils::CaseInsensitiveCompare> headers = {
      {"Host", "aa"}, {"Transfer-Encoding", "chunked"}};
  HttpRequest expect(config::GET, "/html", "HTTP/1.1", headers, "",
                     std::string("0123456789") + "0123456789" + "0123456789" + "0123456789" + "42",
                     HttpRequest::PARSE_COMPLETE);

  // test
  std::string rawRequest =
      "GET /html HTTP/1.1\r\n"
      "Host: aa\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "2A\r\n"  // 42 in decimal
      "0123456789"
      "0123456789"
      "0123456789"
      "0123456789"
      "42\r\n"
      "0\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);
  checkHttpRequestEqual(expect, test);
  test::teardownMaxBodySize();
}
/* -------------- body chunk test end -------------- */
