#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "conf.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ConfigHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "FileUtils.hpp"
#include <iostream>

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
		else {
			config::Main	*config = new config::Main();
			config_handler_.loadConfiguration(config);
			GTEST_SKIP();
		}

		const config::Main*	config = config::initConfig(file_path);
		config_handler_.loadConfiguration(config);
	}

	void TearDown() override {
		// テストケースのクリーンアップ処理
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();
		delete config_handler_.config_;
	}

	ConfigHandler	config_handler_;
	HttpRequest	request_;
};

namespace test {
bool	CORRECT_RESPONSE( const std::vector<std::string>& correct_res, const std::string& response )
{
	for (size_t i = 0; i < correct_res.size(); i++)
	{
		if (response.find(correct_res[i]) == std::string::npos)
		{
			// もしvec_resの要素がresに含まれていなければfalseを返す
			std::cerr << "correct_res: " << correct_res[i] << "\nresponse: " << response << std::endl;
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
	int sock = 1;
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
	std::string response = HttpResponse::generateResponse(request, tied_server, sock, config_handler_);

	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, response));
}

TEST_F(HttpResponseTest, Return)
{
	// 共通初期化
	// 疑似socket作成
	int sock = 1;
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
	std::string response;

	// test case
	// return code; の場合
	// 初期化
	request.uri = "/code/";
	correct_res.push_back("HTTP/1.1 418 I'm a teapot");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	// 関数適用
	response = HttpResponse::generateResponse(request, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, response));



	// return code text; の場合
	// 初期化
	request.uri = "/code-text/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 404 Not Found");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Type: text/plain");
	correct_res.push_back("Content-Length: 14");
	correct_res.push_back("custom message");
	// 関数適用
	response = HttpResponse::generateResponse(request, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, response));

	// return code URL; の場合
	// 初期化
	request.uri = "/code-url/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 307 Temporary Redirect");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Length: 118");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Location: http://127.0.0.1:7000/code-url.html");
	correct_res.push_back("<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary Redirect</h1></center>\r\n");
	// 関数適用
	response = HttpResponse::generateResponse(request, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, response));

	// return URL; の場合
	// 初期化
	request.uri = "/url/";
	correct_res.clear();
	correct_res.push_back("HTTP/1.1 302 Moved Temporarily");
	correct_res.push_back("Server: webserv/1");
	correct_res.push_back("Connection: keep-alive");
	correct_res.push_back("Content-Length: 92");
	correct_res.push_back("Content-Type: text/html");
	correct_res.push_back("Location: http://127.0.0.1:7000/url.html");
	correct_res.push_back("<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n");
	// 関数適用
	response = HttpResponse::generateResponse(request, tied_server, sock, config_handler_);
	// 結果確認
	ASSERT_TRUE(test::CORRECT_RESPONSE(correct_res, response));
}
