#include <gtest/gtest.h>
#include "HttpMessage.hpp"

bool areHttpRequestEqual( HttpRequest &a, HttpRequest &b )
{
	return a.uri == b.uri && a.method == b.method && a.version == b.version;
}

TEST(requestParserTest, GET)
{
	/* test1 */
	std::string rawRequest1 = std::string("GET /data/index.html HTTP/1.1");
	HttpRequest expect1 = HttpRequest{"GET", "/data/index.html", "HTTP/1.1"};
	HttpRequest test1 = HttpMessage::requestParser( rawRequest1 ); 
	EXPECT_TRUE( areHttpRequestEqual( expect1, test1 ) );

	/* test2 */
	std::string rawRequest2 = std::string("POST /data/index.html HTTP/1.1");
	HttpRequest expect2 = HttpRequest{"GET", "/data/index.html", "HTTP/1.1"};
	HttpRequest test2 = HttpMessage::requestParser( rawRequest2 ); 
	EXPECT_FALSE( areHttpRequestEqual( expect2, test2 ) );
}
int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
