#include <gtest/gtest.h>
#include "HttpRequest.hpp"
#include <iostream>

bool printError(std::string method)
{
    std::cerr << "method not equal" << std::endl;
    return false;
}

bool checkHttpRequestEqual(HttpRequest expect, HttpRequest test)
{
    if ( expect.method != test.method )
	return printError("method");
    if ( expect.uri != test.uri )
	return printError("uri");
    if ( expect.version != test.version )
	return printError("version");
    if ( expect.headers != test.headers )
	return printError("headers");
    if ( expect.queries != test.queries )
	return printError("queries");
    if ( expect.body != test.body )
	return printError("body");
    return true;
}

TEST(HttpRequest, HttpRequest)
{
    std::string rawRequest = "GET / HTTP/1.1\r\n:\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    headers.insert(std::make_pair("", ""));
    queries.insert(std::make_pair("", ""));
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "");
    EXPECT_TRUE(checkHttpRequestEqual(test, expect));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

