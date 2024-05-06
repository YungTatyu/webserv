#include <gtest/gtest.h>

#include <vector>

#include "HttpRequest.hpp"

/* --------------  request line error test -------------- */

TEST(HttpRequest, ErrorTest1) {
  // test method invalid
  std::string rawRequest =
      "111 / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest2) {
  // test invalid character in request line
  std::string rawRequest =
      "GET/HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest3) {
  // test uri not begin with slash
  std::string rawRequest =
      "GET uri HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest4) {
  // test invalid character in request line
  std::string rawRequest =
      "GET\n/ HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest5) {
  // test format error (no uri)
  std::string rawRequest =
      "GET HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest6) {
  // test format error (no version)
  std::string rawRequest =
      "GET / \r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest7) {
  // test format error (null)
  std::vector<unsigned char> v = {'\0'};
  std::string rawRequest(v.begin(), v.end());
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest8) {
  // test format error (no status line)
  std::string rawRequest =
      "\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest9) {
  // test format error (no \n at the end of status line)
  std::string rawRequest =
      "GET / HTTP/1.1\r"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest10) {
  // test format error (incomplete method name)
  std::string rawRequest =
      "GE / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest11) {
  // test format error (incomplete method name)
  std::string rawRequest =
      "GETT / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest12) {
  // test format error (no method)
  std::string rawRequest =
      " / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest13) {
  // test format error (get method is not uppercase, (method is case-sensitive))
  std::string rawRequest =
      "get / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest14) {
  // test format error (version is not uppercase, (version is case-sensitive))
  std::string rawRequest =
      "GET / http/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest15) {
  // test format error (version is not uppercase, (version is case-sensitive))
  std::string rawRequest =
      "GET / http/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* --------------  request line error test end -------------- */

/* --------------  header field error test -------------- */

TEST(HttpRequest, ErrorTest16) {
  // test: no Host header field (Host header filed is necessary)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest17) {
  // test: duplicate header filed (header name should be unique)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\nHost: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest18) {
  // test: duplicate header filed (header name should be unique with case insensitive)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\nHOST: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest19) {
  // test: duplicate header filed (header name should be unique with case insensitive)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\nHOST: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest20) {
  // test: format error (header filed shold be headername(no space):(one or multiple spaces)value(one or
  // multiple spaces) ヘッダーネームの後には空白があってはならない.すぐにコロン.
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host :example.com\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest21) {
  // test: format error (header name should not start with space)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* --------------  header field error test end -------------- */

/* -------------- query string error test -------------- */

TEST(HttpRequest, ErrorTest22) {
  // test: format error (no uri and just query string)
  std::string rawRequest =
      "GET ?name=valueHTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* -------------- query string error test end -------------- */

/* -------------- uri decode error test -------------- */

TEST(HttpRequest, ErrorTest23) {
  // test: format error (encoded string should be %(digit)(digit)) %の後に一つ足りない。
  std::string rawRequest =
      "GET /Hello%2World HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest24) {
  // test: format error (encoded string should be %(digit)(digit)) %のあとに二つ足りない。
  std::string rawRequest =
      "GET /Hello%World HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest25) {
  // test: format error (encoded string should be %(hexdigit)(hexdigit))
  std::string rawRequest =
      "GET /Hello%ahWorld HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest26) {
  // test: format error (encoded string should be %(hexdigit)(hexdigit))
  std::string rawRequest =
      "GET /Hello%aHWorld HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* -------------- uri decode error test end -------------- */

/* -------------- header host test -------------- */
TEST(HttpRequest, ErrorTest27) {
  // test: empty
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: \r\n"
      "New: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}
/* -------------- header host test end -------------- */

/* -------------- header content-length test -------------- */
TEST(HttpRequest, ErrorTest28) {
  // test: bigger than longmax
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: 9223372036854775808\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest29) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: 0.0\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest30) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: -1\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest31) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: 1.1\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest32) {
  // test: empty value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest33) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: l\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest34) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest35) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length:\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest36) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length:test\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest37) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length:1A\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest38) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length:+0\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest39) {
  // test: invalid value
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length:-0\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* -------------- header content-length test end -------------- */

TEST(HttpRequest, ErrorTest40) {
  // test: dup content-length, transfer-encoding
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "content-length: 2\r\n"
      "transfer-encoding: chunked\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest41) {
  // test: dup content-length, transfer-encoding
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "transfer-encoding: chunked\r\n"
      "content-length: 2\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* -------------- header transfer-encoding test -------------- */

TEST(HttpRequest, ErrorTest42) {
  // test: transfer-encoding not implemented
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "transfer-encoding: \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parseState);
}

TEST(HttpRequest, ErrorTest43) {
  // test: transfer-encoding not implemented
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "transfer-encoding\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parseState);
}

TEST(HttpRequest, ErrorTest44) {
  // test: transfer-encoding not implemented
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "transfer-encoding:\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parseState);
}

TEST(HttpRequest, ErrorTest45) {
  // test: transfer-encoding not implemented
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "transfer-encoding:random \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parseState);
}

/* -------------- header transfer-encoding test end -------------- */
