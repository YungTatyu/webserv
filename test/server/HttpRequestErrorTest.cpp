#include <gtest/gtest.h>
#include "HttpRequest.hpp"

/* --------------  request line error test -------------- */

TEST(HttpRequest, ErrorTest1)
{
    //test method invalid
    std::string rawRequest = "111 / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest2)
{
    //test invalid character in request line
    std::string rawRequest = "GET/HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest3)
{
    //test invalid character in request line
    std::string rawRequest = "GET\n/ HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest4)
{
    //test format error (no uri)
    std::string rawRequest = "GET HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest5)
{
    //test format error (no version)
    std::string rawRequest = "GET / \r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest6)
{
    //test format error (empty)
    std::string rawRequest = "";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest7)
{
    //test format error (no status line)
    std::string rawRequest = "\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}


TEST(HttpRequest, ErrorTest8)
{
    //test format error (no \n at the end of status line)
    std::string rawRequest = "GET / HTTP/1.1\r" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest9)
{
    //test format error (incomplete method name)
    std::string rawRequest = "GE / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest10)
{
    //test format error (incomplete method name)
    std::string rawRequest = "GETT / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest11)
{
    //test format error (no method)
    std::string rawRequest = " / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest12)
{
    //test format error (get method is not uppercase, (method is case-sensitive))
    std::string rawRequest = "get / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest13)
{
    //test format error (version is not uppercase, (version is case-sensitive))
    std::string rawRequest = "GET / http/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest14)
{
    //test format error (version is not uppercase, (version is case-sensitive))
    std::string rawRequest = "GET / http/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* --------------  request line error test end -------------- */

/* --------------  header field error test -------------- */

TEST(HttpRequest, ErrorTest15)
{
    //test: no Host header field (Host header filed is necessary)
    std::string rawRequest = "GET / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest16)
{
    //test: duplicate header filed (header name should be unique)
    std::string rawRequest = "GET / HTTP/1.1\r\n" "Host: example.com\r\nHost: aa\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest17)
{
    //test: duplicate header filed (header name should be unique with case insensitive)
    std::string rawRequest = "GET / HTTP/1.1\r\n" "Host: example.com\r\nHOST: aa\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest18)
{
    //test: duplicate header filed (header name should be unique with case insensitive)
    std::string rawRequest = "GET / HTTP/1.1\r\n" "Host: example.com\r\nHOST: aa\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest19)
{
    //test: format error (header filed shold be headername(no space):(one or multiple spaces)value(one or multiple spaces) 
    //ヘッダーネームの後には空白があってはならない.すぐにコロン.
    std::string rawRequest = "GET / HTTP/1.1\r\n" "Host :example.com\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

/* --------------  header field error test end -------------- */

//重複するヘッダーがあったらどうするんだっけ -> error (caseinsensitive)
//Hostヘッダーフィールドがなければエラー
//Content-Lengthと実際のボディのサイズが正しいかとか？
//chunkだと？
