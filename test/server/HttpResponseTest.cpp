#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "conf.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ConfigHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>

/*
 * ===== HttpResponseTest 方針 =====
 *
 *
 * テストごとに、requestとconfigファイルと想定するresponseを用意する
 * generateResponse関数を適用して、出力が想定通りか確認する。
 *
 */


class HttpResponseTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path;
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();
		if (static_cast<std::string>(test_info->name()) == "ParseError") {
			file_path = "test/server/ResponseTestFiles/ParseError.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "Return") {
			file_path = "test/server/ResponseTestFiles/Return.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "ErrorPage") {
			file_path = "test/server/ResponseTestFiles/ErrorPage.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "StaticHandler") {
			file_path = "test/server/ResponseTestFiles/staticHandler.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "autoIndex") {
			file_path = "test/server/ResponseTestFiles/autoIndex.conf";
		}
		else {
			config::Main	*config = new config::Main();
			config_handler_.loadConfiguration(config);
			GTEST_SKIP();
		}

		const config::Main*	config = config::initConfig(file_path);
		config_handler_.loadConfiguration(config);

		if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockfd) == -1)
		{
			perror("socketpair");
			delete config;
			GTEST_SKIP();
		}
	}

	void TearDown() override {
		// テストケースのクリーンアップ処理
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();

		// config handler　削除
		delete config_handler_.config_;

		// socket 処理
		close(sockfd[0]);
		close(sockfd[1]);
	}

	ConfigHandler	config_handler_;
	HttpRequest	request_;
	int	sockfd[2];
};

namespace test {
bool	CORRECT_RESPONSE( const std::vector<std::string>& correct_res, const std::string& final_response )
{
	for (size_t i = 0; i < correct_res.size(); i++)
	{
		if (final_response.find(correct_res[i]) == std::string::npos)
		{
			// もしvec_resの要素がresに含まれていなければfalseを返す
			std::cerr << "correct_res: " << correct_res[i] << "\nresponse: " << final_response << "\n\n";
			return false;
		}
	}
	// 全ての要素が含まれている場合はtrueを返す
	return true;
}
};

TEST_F(HttpResponseTest, ParseError)
{
	// 初期化
	int sock = this->sockfd[0];
	HttpResponse	response;
	// 正解のレスポンスを初期化 なぜかdefault_errorpage_map_が型エラーになったのでここでは直書き
	std::vector<std::string> correct_res;
	correct_res.push_back("400 Bad Request");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n");
	// HttpRequest作成
	HttpRequest	request;

	request.method = "GET";
	request.headers["Host"] = "test_server";
	request.uri = "/";
	request.version = "HTTP/1.1";
	request.parseState = HttpRequest::PARSE_ERROR;
	// TiedServer 作成
	struct TiedServer	tied_server = config_handler_.createTiedServer("127.0.0.1", 8001);

	// 関数適用
	std::string final_response = HttpResponse::generateResponse(request, response, tied_server, sock, config_handler_);

	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));
}

TEST_F(HttpResponseTest, Return)
{
	// 共通初期化
	// 疑似socket作成
	int sock = this->sockfd[0];
	// HttpRequest作成
	HttpRequest	request;

	request.method = "GET";
	request.version = "HTTP/1.1";
	request.headers["Host"] = "test_server";
	request.parseState = HttpRequest::PARSE_COMPLETE;
	// TiedServer 作成
	struct TiedServer	tied_server = config_handler_.createTiedServer("127.0.0.1", 8001);
	// 正解response
	std::vector<std::string> correct_res;
	std::string final_response;

	// test case
	// return code; の場合
	// 初期化
	HttpResponse	response1;
	request.uri = "/code/";
	correct_res.push_back("HTTP/1.1 418 I'm a teapot");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response1, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));



	// return code text; の場合
	// 初期化
	HttpResponse	response2;
	request.uri = "/code-text/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 404 Not Found");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/plain");
	correct_res.push_back("Content-Length: 14");
	correct_res.push_back("custom message");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response2, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// return code URL; の場合
	// 初期化
	HttpResponse	response3;
	request.uri = "/code-url/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 307 Temporary Redirect");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Length: 168");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Location: http://127.0.0.1:7000/code-url.html");
	correct_res.push_back("<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary Redirect</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response3, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// return URL; の場合
	// 初期化
	HttpResponse	response4;
	request.uri = "/url/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 302 Moved Temporarily");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Length: 142");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Location: http://127.0.0.1:7000/url.html");
	correct_res.push_back("<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response4, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));
}

TEST_F(HttpResponseTest, ErrorPage)
{
	// 共通初期化
	// 疑似socket作成
	int sock = this->sockfd[0];
	// HttpRequest作成
	HttpRequest	request;

	request.method = "GET";
	request.version = "HTTP/1.1";
	request.headers["Host"] = "test_server";
	request.parseState = HttpRequest::PARSE_COMPLETE;
	// TiedServer 作成
	struct TiedServer	tied_server = config_handler_.createTiedServer("127.0.0.1", 8001);
	// 正解response
	std::vector<std::string> correct_res;
	std::string final_response;

	// test case
	// error_page code ... =response uri;
	// 初期化
	HttpResponse	response1;
	request.uri = "/nothing.html";
	correct_res.push_back("HTTP/1.1 499");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 258");
	std::ifstream	ifs("test/server/ResponseTestFiles/testHtml/40x.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response1, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));


	// error_page code ... uri; の場合
	// 初期化
	HttpResponse	response2;
	request.uri = "/bad-request/";
	request.parseState = HttpRequest::PARSE_ERROR;
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 400 Bad Request");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 274");
	ifs.clear();
	ifs.open("test/server/ResponseTestFiles/testHtml/internal_redirect.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer2;
	buffer2 << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer2.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response2, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// 無限リダイレクトの場合
	// 初期化
	HttpResponse	response3;
	request.uri = "/permanently_internal_redirect/";
	request.parseState = HttpRequest::PARSE_COMPLETE;
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 500 Internal Server Error");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: close");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 174");
	correct_res.push_back("<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body>\r\n<center><h1>500 Internal Server Error</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response3, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));
}

TEST_F(HttpResponseTest, StaticHandler)
{
	// 共通初期化
	// 疑似socket作成
	int sock = this->sockfd[0];
	// HttpRequest作成
	HttpRequest	request;

	request.method = "GET";
	request.version = "HTTP/1.1";
	request.headers["Host"] = "test_server";
	request.parseState = HttpRequest::PARSE_COMPLETE;
	// TiedServer 作成
	struct TiedServer	tied_server = config_handler_.createTiedServer("127.0.0.1", 8001);
	// 正解response
	std::vector<std::string> correct_res;
	std::string final_response;

	// test case
	// root ディレクトリ
	// 初期化
	HttpResponse	response1;
	request.uri = "/";
	correct_res.push_back("HTTP/1.1 200 OK");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 272");
	std::ifstream	ifs("test/server/ResponseTestFiles/testHtml/index.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response1, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// url がディレクトリだけど'/'で終わってない
	// 初期化
	HttpResponse	response2;
	request.uri = "/testHtml";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 301 Moved Permanently");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 166");
	correct_res.push_back("<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved Permanently</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response2, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// urlが存在しないディレクトリ
	// 初期化
	HttpResponse	response3;
	request.uri = "/nothing/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 404 Not Found");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 150");
	correct_res.push_back("<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not Found</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response3, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// try_filesとaliasの組み合わせ
	// 初期化
	HttpResponse	response4;
	request.uri = "/alias/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 200 OK");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 286");
	ifs.clear();
	ifs.open("test/server/ResponseTestFiles/aliasHtml/alias.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer3;
	buffer3 << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer3.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response4, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// indexとtry_filesの組み合わせ
	// 初期化
	HttpResponse	response5;
	request.uri = "/location-root/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 200 OK");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 272");
	ifs.clear();
	ifs.open("test/server/ResponseTestFiles/testHtml/index.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer4;
	buffer4 << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer4.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response5, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// try_filesのinternal redirect
	// 初期化
	request.uri = "/redirect/";
	HttpResponse	response6;
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 200 OK");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 274");
	ifs.clear();
	ifs.open("test/server/ResponseTestFiles/testHtml/internal_redirect.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer5;
	buffer5 << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer5.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response6, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// try_filesの=code からerror_pageの場合
	// 初期化
	HttpResponse	response7;
	request.uri = "/code-error-page/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 405 Not Allowed");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 272");
	ifs.clear();
	ifs.open("test/server/ResponseTestFiles/testHtml/index.html");
	ASSERT_TRUE(ifs.is_open());
	std::stringstream buffer6;
	buffer6 << ifs.rdbuf();
	ifs.close();
	correct_res.push_back(buffer6.str());
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response7, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));
}

TEST_F(HttpResponseTest, autoIndex)
{
	// 共通初期化
	// 疑似socket作成
	int sock = this->sockfd[0];
	// HttpRequest作成
	HttpRequest	request;

	request.method = "GET";
	request.version = "HTTP/1.1";
	request.headers["Host"] = "first_server";
	request.parseState = HttpRequest::PARSE_COMPLETE;
	// TiedServer 作成
	struct TiedServer	tied_server = config_handler_.createTiedServer("127.0.0.1", 8001);
	// 正解response
	std::vector<std::string> correct_res;
	std::string final_response;

	// test case 1
	// http contextのautoindex off, location contextのautoindex on
	HttpResponse	response1;
	request.uri = "/";
	correct_res.push_back("HTTP/1.1 200 OK");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Index of");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response1, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// test case 2
	// http off, location ディレクティブなし
	HttpResponse	response2;
	request.uri = "/no-autoindex/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 403 Forbidden");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 150");
	correct_res.push_back("<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response2, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));


	// second_server
	request.headers["Host"] = "second_server";
	struct TiedServer	tied_server2 = config_handler_.createTiedServer("127.0.0.1", 8002);

	// test case 3
	// http contextのautoindex off, server contextのautoindex on, location context 中身無し。
	HttpResponse	response3;
	request.uri = "/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 200 OK");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Index of");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response3, tied_server2, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));

	// test case 4
	// 直近autoindex on, locationの directory 存在しない
	HttpResponse	response4;
	request.uri = "/nothing-directory/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 404 Not Found");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Content-Length: 150");
	correct_res.push_back("<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not Found</h1></center>\r\n<hr><center>webserv/1</center>\r\n</body>\r\n</html>\r\n");
	// 関数適用
	final_response = HttpResponse::generateResponse(request, response4, tied_server2, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, final_response));
}
