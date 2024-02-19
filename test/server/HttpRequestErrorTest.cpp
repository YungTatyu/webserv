#include <gtest/gtest.h>
#include "HttpRequest.hpp"

TEST(HttpRequest, ErrorTest1)
{
    //test method invalid
    std::string rawRequest = "111 / HTTP/1.1\r\n" "\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

TEST(HttpRequest, ErrorTest2)
{
    //test invalid character in request line
    std::string rawRequest = "GET\n/ HTTP/1.1\r\n" "\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    EXPECT_EQ(HttpRequest::PARSE_ERROR, test.parseState);
}

