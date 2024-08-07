#include <gtest/gtest.h>

#include <vector>

#include "HttpRequest.hpp"
#include "cli_max_body_size_test.hpp"

/* --------------  request line error test -------------- */

TEST(HttpRequest, ErrorTest1) {
  // test method invalid
  std::string rawRequest =
      "111 / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest2) {
  // test invalid character in request line
  std::string rawRequest =
      "GET/HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest3) {
  // test uri not begin with slash
  std::string rawRequest =
      "GET uri HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest4) {
  // test invalid character in request line
  std::string rawRequest =
      "GET\n/ HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest5) {
  // test format error (no uri)
  std::string rawRequest =
      "GET HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest6) {
  // test format error (no version)
  std::string rawRequest =
      "GET / \r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest7) {
  // test format error (null)
  std::vector<unsigned char> v = {'\0'};
  std::string rawRequest(v.begin(), v.end());
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest8) {
  // test format error (no status line)
  std::string rawRequest =
      "\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest9) {
  // test format error (no \n at the end of status line)
  std::string rawRequest =
      "GET / HTTP/1.1\r"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest10) {
  // test format error (incomplete method name)
  std::string rawRequest =
      "GE / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest11) {
  // test format error (incomplete method name)
  std::string rawRequest =
      "GETT / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest12) {
  // test format error (no method)
  std::string rawRequest =
      " / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest13) {
  // test format error (get method is not uppercase, (method is case-sensitive))
  std::string rawRequest =
      "get / HTTP/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest14) {
  // test format error (version is not uppercase, (version is case-sensitive))
  std::string rawRequest =
      "GET / http/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest15) {
  // test format error (version is not uppercase, (version is case-sensitive))
  std::string rawRequest =
      "GET / http/1.1\r\n"
      "Host: a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest17) {
  // test: duplicate header filed (header name should be unique)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\nHost: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest18) {
  // test: duplicate header filed (header name should be unique with case insensitive)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\nHOST: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest19) {
  // test: duplicate header filed (header name should be unique with case insensitive)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\nHOST: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest20) {
  // test: format error (header filed shold be headername(no space):(one or multiple spaces)value(one or
  // multiple spaces) ヘッダーネームの後には空白があってはならない.すぐにコロン.
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host :example.com\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest21) {
  // test: format error (header name should not start with space)
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest24) {
  // test: format error (encoded string should be %(digit)(digit)) %のあとに二つ足りない。
  std::string rawRequest =
      "GET /Hello%World HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest25) {
  // test: format error (encoded string should be %(hexdigit)(hexdigit))
  std::string rawRequest =
      "GET /Hello%ahWorld HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest26) {
  // test: format error (encoded string should be %(hexdigit)(hexdigit))
  std::string rawRequest =
      "GET /Hello%aHWorld HTTP/1.1\r\n"
      "Host: example.com\r\n New: aa\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, invalid_host1) {
  // test: empty
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt tt\r\n"
      "New: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, invalid_host2) {
  // test: empty
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host:a   a\r\n"
      "New: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parse_state_);
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

  EXPECT_EQ(HttpRequest::PARSE_NOT_IMPLEMENTED, test.parse_state_);
}

/* -------------- header transfer-encoding test end -------------- */

/* -------------- header unique test -------------- */

TEST(HttpRequest, ErrorTest46) {
  // test: dup header
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "transfer-encoding:chunked \r\n"
      "transfer-encoding:chunked \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest47) {
  // test: dup header
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "Host: yay\r\n"
      "transfer-encoding:chunked \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, ErrorTest48) {
  // test: dup header
  std::string rawRequest =
      "GET / HTTP/1.1\r\n"
      "Host: tt\r\n"
      "Host: yay\r\n"
      "content-length: 2 \r\n"
      "content-length: 10 \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(rawRequest, test);

  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

/* -------------- header unique test end -------------- */

/* -------------- header uri test -------------- */

TEST(HttpRequest, error_normalize_uri_1) {
  // test: dotdot
  std::string req =
      "GET /.. HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_normalize_uri_2) {
  // test: dotdot
  std::string req =
      "GET /../ HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_normalize_uri_3) {
  // test: dotdot
  std::string req =
      "GET /path/../.. HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_normalize_uri_4) {
  // test: dotdot
  std::string req =
      "GET /path/../../.. HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_normalize_uri_5) {
  // test: go crazy!
  std::string req =
      "GET //.///./////.../..//..../////../path/../../.../..//to//../script///..?query "
      "HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_normalize_uri_6) {
  // test: go crazy!
  std::string req =
      "GET //..//..///...///..../path///to//../script///..?query HTTP/1.1\r\n"
      "Host: aa\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}
/* -------------- header uri test end -------------- */

/* -------------- client max body size test -------------- */
TEST(HttpRequest, error_cli_max_body_size_1) {
  // test: content-length too long
  test::setupMaxBodySize(100);
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "content-length: 100\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR_BODY_TOO_LARGE, test.parse_state_);
  test::teardownMaxBodySize();
}

TEST(HttpRequest, error_cli_max_body_size_2) {
  // test: chunked bytes too large
  test::setupMaxBodySize(100);
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked\r\n"
      "\r\n"
      "1\r\n"
      "a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parse_state_);

  req =
      "63\r\n"  // 99 bytes in dec
      "this is test";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR_BODY_TOO_LARGE, test.parse_state_);
  test::teardownMaxBodySize();
}

TEST(HttpRequest, error_cli_max_body_size_3) {
  // test: chunked bytes too large
  test::setupMaxBodySize(5);
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked\r\n"
      "\r\n"
      "1\r\n"
      "a\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parse_state_);

  req =
      "3\r\n"
      "123\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_INPROGRESS, test.parse_state_);

  req =
      "1\r\n"
      "t\n"
      "\r\n";
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR_BODY_TOO_LARGE, test.parse_state_);
  test::teardownMaxBodySize();
}
/* -------------- client max body size test end -------------- */

/* -------------- chunked body test -------------- */
TEST(HttpRequest, error_chunked_body_1) {
  // test: chunked date is bigger than expected
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "123456\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_chunked_body_2) {
  // test: need nl after data
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked\r\n"
      "\r\n"
      "5\r\n"
      "12345"
      "0\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_chunked_body_3) {
  // test: need nl  after bytes
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: aa\r\n"
      "transfer-encoding: chunked\r\n"
      "\r\n"
      "5"
      "this \r\n"
      "0\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}
/* -------------- chunked body test end -------------- */

/* -------------- host with port test -------------- */
TEST(HttpRequest, error_host_1) {
  // test: value is :
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: :\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_host_2) {
  // test: value is :
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host::\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_host_3) {
  // test: value starts with :
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: :hostname\r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}

TEST(HttpRequest, error_host_4) {
  // test: value starts with :
  std::string req =
      "GET / HTTP/1.1\r\n"
      "Host: :::  \r\n"
      "\r\n";
  HttpRequest test;
  HttpRequest::parseRequest(req, test);
  EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parse_state_);
}
/* -------------- host with port test end -------------- */
