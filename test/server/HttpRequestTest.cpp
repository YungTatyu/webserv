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
    //リクエストラインだけ
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "");

    //test
    std::string rawRequest = "GET / HTTP/1.1\r\n\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test2)
{
    //header fieldが一対ある時
    //bodyもある
    std::map<std::string, std::string> headers;
    headers["name1"] = "value1";
    std::map<std::string, std::string> queries;
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "this is body");

    //test
    std::string rawRequest = "GET / HTTP/1.1\r\nname1:value1\r\n\r\nthis is body";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test3)
{
    //header fieldが複数ある時
    std::map<std::string, std::string> headers;
    headers["name1"] = "value1";
    headers["name2"] = "value2";
    std::map<std::string, std::string> queries;
    HttpRequest expect("GET", "/", "HTTP/1.1", headers, queries, "");

    //test
    std::string rawRequest = "GET / HTTP/1.1\r\nname1:value1\r\nname2:value2\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test4)
{
    //query stringが単体
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    queries["query1"] = "value1";
    HttpRequest expect("GET", "/html", "HTTP/1.1", headers, queries, "");

    //test
    std::string rawRequest = "GET /html?query1=value1 HTTP/1.1\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, Test5)
{
    //query stringが複数ある時
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queries;
    queries["query1"] = "value1";
    queries["query2"] = "value2";
    HttpRequest expect("GET", "/html", "HTTP/1.1", headers, queries, "");

    //test
    std::string rawRequest = "GET /html?query1=value1&query2=value2 HTTP/1.1\r\n\r\n";
    HttpRequest test = HttpRequest::parseRequest(rawRequest);

    checkHttpRequestEqual(expect, test);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

