#include <gtest/gtest.h>
#include "HttpRequest.hpp"

// リクエストライン
TEST(HttpRequestTest, ParseRequestLine)
{
    std::string rawRequest = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";
    HttpRequest request = HttpRequest::parseRequest(rawRequest);
    EXPECT_EQ("GET", request.method);
    EXPECT_EQ("/index.html", request.uri);
    EXPECT_EQ("HTTP/1.1", request.version);
}

// ヘッダー
TEST(HttpRequestTest, ParseHeaders)
{
    std::string rawRequest = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    HttpRequest request = HttpRequest::parseRequest(rawRequest);
    EXPECT_EQ("example.com", request.headers["Host"]);
    EXPECT_EQ("close", request.headers["Connection"]);
}

// ボディ
TEST(HttpRequestTest, ParseBody)
{
    std::string rawRequest = "POST /submit HTTP/1.1\r\nContent-Length: 13\r\n\r\nHello=World!";
    HttpRequest request = HttpRequest::parseRequest(rawRequest);
    EXPECT_EQ("Hello=World!", request.body);
}

// クエリストリング
TEST(HttpRequestTest, ParseQuery)
{
    std::string rawRequest = "GET /search?q=Google&lang=en HTTP/1.1\r\nHost: example.com\r\n\r\n";
    HttpRequest request = HttpRequest::parseRequest(rawRequest);
    EXPECT_EQ("Google", request.queries["q"]);
    EXPECT_EQ("en", request.queries["lang"]);
}

// URLデコード
TEST(HttpRequestTest, UrlDecode)
{
    HttpRequest request;
    std::string encoded = "Hello%20World%21";
    std::string decoded = request.urlDecode(encoded);
    EXPECT_EQ("Hello World!", decoded);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

