#include <gtest/gtest.h>
#include "HttpRequest.hpp"

void checkHttpRequestEqual(HttpRequest expect, HttpRequest test)
{
    EXPECT_EQ( expect.method, test.method );
    EXPECT_EQ( expect.uri, test.uri );
    EXPECT_EQ( expect.version, test.version );
    EXPECT_EQ( expect.headers, test.headers );
    EXPECT_EQ( expect.queries, test.queries );
    EXPECT_EQ( expect.body, test.body );
}

TEST(HttpRequest, Test1)
{
    std::string rawRequest = "GET / HTTP/1.1\r\n\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    headers.insert(std::make_pair("", ""));
    queries.insert(std::make_pair("", ""));
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "");
    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test2)
{
    // header fieldが一対ある時
    // bodyもある
    std::string rawRequest = "GET / HTTP/1.1\r\nname1:value1\r\n\r\nthis is body";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    headers.insert(std::make_pair("name1", "value1"));
    queries.insert(std::make_pair("", ""));
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "this is body");
    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test3)
{
    //header fieldが複数ある時
    std::string rawRequest = "GET / HTTP/1.1\r\nname1:value1\r\nname2:value2\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    headers.insert(std::make_pair("name1", "value1"));
    headers.insert(std::make_pair("name2", "value2"));
    queries.insert(std::make_pair("", ""));
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "");
    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test4)
{
    //query stringが単体
    std::string rawRequest = "GET /html?query1=value1 HTTP/1.1\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    headers.insert(std::make_pair("", ""));
    queries.insert(std::make_pair("query1", "value1"));
    HttpRequest expect("GET", "/html", "HTTP/1.1", headers, queries, "");
    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test5)
{
    //query stringが複数ある時
    std::string rawRequest = "GET /html?query1=value1&query2=value2 HTTP/1.1\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    headers.insert(std::make_pair("", ""));
    queries.insert(std::make_pair("query1", "value1"));
    queries.insert(std::make_pair("query2", "value2"));
    HttpRequest expect("GET", "/html", "HTTP/1.1", headers, queries, "");
    checkHttpRequestEqual(expect, test);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

