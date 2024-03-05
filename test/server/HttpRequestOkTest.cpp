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

    EXPECT_EQ( expect.parseState, test.parseState );
}

TEST(HttpRequest, OkTest1)
{
    //testcase: リクエストラインだけ
    std::map<std::string, std::string> headers;
    HttpRequest expect(GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

    //test
    std::string rawRequest = "GET / HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest2)
{
    //testcase: header fieldが一対ある時
    //testcase: bodyもある
    std::map<std::string, std::string> headers;
    headers["name1"] = "value1";
    HttpRequest expect(GET, "/", "HTTP/1.1", headers, "", "this is body", HttpRequest::PARSE_COMPLETE);

    //test
    std::string rawRequest = "GET / HTTP/1.1\r\n"
                             "name1:value1\r\n"
                             "\r\n"
                             "this is body";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest3)
{
    //testcase: header fieldが複数ある時
    std::map<std::string, std::string> headers;
    headers["name1"] = "value1";
    headers["name2"] = "value2";
    HttpRequest expect(GET, "/", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

    //test
    std::string rawRequest = "GET / HTTP/1.1\r\n"
                             "name1:value1\r\n"
                             "name2:value2\r\n"
                             "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest4)
{
    //testcase: query stringが単体
    std::map<std::string, std::string> headers;
    HttpRequest expect(GET, "/html", "HTTP/1.1", headers, "query1=value1", "", HttpRequest::PARSE_COMPLETE);

    //test
    std::string rawRequest = "GET /html?query1=value1 HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest5)
{
    //testcase: query stringが複数ある時
    std::map<std::string, std::string> headers;
    HttpRequest expect(GET, "/html", "HTTP/1.1", headers, "query1=value1&query2=value2", "", HttpRequest::PARSE_COMPLETE);

    //test
    std::string rawRequest = "GET /html?query1=value1&query2=value2 HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest6)
{
    //testcase: chunked first
    std::map<std::string, std::string> headers;
    headers["Transfer-Encoding"] = "chunked";
    HttpRequest expect(GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

    //test
    std::string rawRequest = "GET /html HTTP/1.1\r\n"
                             "Transfer-Encoding: chunked\r\n"
                             "\r\n"
                             "5\r\n"
                             "hello"
                             "\r\n";
    std::cout << "phase 1" << std::endl;
    HttpRequest test;
    std::cout << "phase 2" << std::endl;
    HttpRequest::parseRequest(rawRequest, test);
    std::cout << "phase 3" << std::endl;

    checkHttpRequestEqual(expect, test);
}

TEST(HttpRequest, OkTest7)
{
    //testcase: chunked first
    std::map<std::string, std::string> headers;
    headers["Transfer-Encoding"] = "chunked";
    HttpRequest expect(GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

    //test
    std::string rawRequest = "GET /html HTTP/1.1\r\n"
                             "Transfer-Encoding: chunked\r\n"
                             "\r\n"
                             "5\r\n"
                             "hello"
                             "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    //testcase: chunked second
    HttpRequest expect2(GET, "/html", "HTTP/1.1", headers, "", "hello world", HttpRequest::PARSE_INPROGRESS);

    //test
    std::string chunked = "6\r\n" " world" "\r\n";
    HttpRequest::parseRequest(chunked, test);

    checkHttpRequestEqual(expect2, test);
}

TEST(HttpRequest, OkTest8)
{
    //testcase: chunked first
    std::map<std::string, std::string> headers;
    headers["Transfer-Encoding"] = "chunked";
    HttpRequest expect(GET, "/html", "HTTP/1.1", headers, "", "hello", HttpRequest::PARSE_INPROGRESS);

    //test
    std::string rawRequest = "GET /html HTTP/1.1\r\n"
                             "Transfer-Encoding: chunked\r\n"
                             "\r\n"
                             "5\r\n"
                             "hello"
                             "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    //testcase: chunked second
    HttpRequest expect2(GET, "/html", "HTTP/1.1", headers, "", "hello world", HttpRequest::PARSE_INPROGRESS);

    //test
    std::string chunked = "6\r\n" " world" "\r\n";
    HttpRequest::parseRequest(chunked, test);

    //testcase: chunked third (end)
    HttpRequest expect3(GET, "/html", "HTTP/1.1", headers, "", "hello world", HttpRequest::PARSE_COMPLETE);

    //test
    std::string chunked2 = "0\r\n" "\r\n";
    HttpRequest::parseRequest(chunked2, test);
 
    checkHttpRequestEqual(expect3, test);
}

TEST(HttpRequest, OkTest9)
{
    //testcase: encoded url
    std::map<std::string, std::string> headers;
    HttpRequest expect(GET, "/Hello World!", "HTTP/1.1", headers, "", "", HttpRequest::PARSE_COMPLETE);

    //test
    std::string rawRequest = "GET /Hello%20World%21 HTTP/1.1\r\n" "\r\n";
    HttpRequest test;
    HttpRequest::parseRequest(rawRequest, test);

    checkHttpRequestEqual(expect, test);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

