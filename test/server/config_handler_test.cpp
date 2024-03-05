#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ConfigHandler.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/param.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

// 各テストで使うconfigをセットするクラス
class ConfigHandlerTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path;
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();
		if (static_cast<std::string>(test_info->name()) == "allowRequest") {
			file_path = "test/server/conf_files/allowRequest_test.conf";
		}
		/*else if (static_cast<std::string>(test_info->name()) == "searchFile") {
			file_path = "test/server/conf_files/searchFile_test.conf";
		}*/
		else if (static_cast<std::string>(test_info->name()) == "searchKeepaliveTimeout") {
			file_path = "test/server/conf_files/searchKeepaliveTimeout_test.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "searchSendTimeout") {
file_path = "test/server/conf_files/searchSendTimeout_test.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "searchUseridExpires") {
			file_path = "test/server/conf_files/searchUseridExpires_test.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "writeAcsLog") {
			file_path = "test/server/conf_files/writeAcsLog_test.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "writeErrLog") {
			file_path = "test/server/conf_files/writeErrLog_test.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "createTiedServer") {
			file_path = "test/server/conf_files/createTiedServer_test.conf";
		}
		else {
			config::Main	*config = new config::Main();
			config_handler_.loadConfiguration(config);
			GTEST_SKIP();
		}

		config::Main	*config = new config::Main();
		config::Lexer	lexer(file_path);
		lexer.tokenize();

		config::Parser	parser(*config, lexer.getTokens(), file_path);
		if (!parser.parse())
			GTEST_SKIP();

		config_handler_.loadConfiguration(config);
	}


	void TearDown() override {
        // テストケースのクリーンアップ処理
		delete config_handler_.config_;
    }

	// テストに使うオブジェクト
	ConfigHandler		config_handler_;
};

namespace test {
// writeAcs/ErrLogのテスト用
bool	WRITE_ACCURATE( const std::string file, const std::string& phrase ) {
	std::ifstream	ifs( file );
	if (!ifs.is_open()) {
		std::cerr << "file open failed." << std::endl;
		return false;
	}

	std::ostringstream content_stream;
	content_stream << ifs.rdbuf();
	std::string content = content_stream.str();

	return ( content.find(phrase) != std::string::npos );
}

bool	sameTiedServer(const struct TiedServer& tied1, const struct TiedServer& tied2)
{
	return (tied1.servers_.size() == tied2.servers_.size() &&
		tied1.addr_ == tied2.addr_ &&
		tied1.port_ == tied2.port_);
}

bool	sameTime(const config::Time& time1, const config::Time& time2)
{
	return (time1.time_in_ms_ == time2.time_in_ms_);
}
};

// allowRequestの引数プラスでclient自身のアドレス必要かも
TEST_F(ConfigHandlerTest, allowRequest)
{
	HttpRequest		request;

	// 初期化
	struct sockaddr_in	cli_addr1;
	std::memset(&cli_addr1, 0, sizeof(cli_addr1)); // ゼロで初期化
	cli_addr1.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "192.168.0.1", &(cli_addr1.sin_addr));

	struct sockaddr_in	cli_addr2;
	std::memset(&cli_addr2, 0, sizeof(cli_addr2)); // ゼロで初期化
	cli_addr2.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "192.168.0.2", &(cli_addr2.sin_addr));

	struct sockaddr_in	cli_addr3;
	std::memset(&cli_addr3, 0, sizeof(cli_addr3)); // ゼロで初期化
	cli_addr3.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "192.168.0.3", &(cli_addr3.sin_addr));

	struct sockaddr_in	cli_addr4;
	std::memset(&cli_addr4, 0, sizeof(cli_addr4)); // ゼロで初期化
	cli_addr4.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "192.168.0.4", &(cli_addr4.sin_addr));


	request.headers["Host"] = "first_server";
	// allow all
	request.uri = "/";
	EXPECT_TRUE(config_handler_.allowRequest(config_handler_.config_->http.server_list[0],
									&config_handler_.config_->http.server_list[0].location_list[0],
									request,
									cli_addr1));
	// deny all
	request.uri = "/hello";
	EXPECT_FALSE(config_handler_.allowRequest(config_handler_.config_->http.server_list[0],
									&config_handler_.config_->http.server_list[0].location_list[1],
									request,
									cli_addr1));

	request.headers["Host"] = "second_server";
	// allow cli_addr2
	request.uri = "/";
	EXPECT_TRUE(config_handler_.allowRequest(config_handler_.config_->http.server_list[1],
									&config_handler_.config_->http.server_list[1].location_list[0],
									request,
									cli_addr2));
	// deny cli_addr2
	request.uri = "/hello";
	EXPECT_FALSE(config_handler_.allowRequest(config_handler_.config_->http.server_list[1],
									&config_handler_.config_->http.server_list[1].location_list[1],
									request,
									cli_addr2));

	request.headers["Host"] = "third_server";
	// deny cli_addr3 
	request.uri = "/";
	EXPECT_FALSE(config_handler_.allowRequest(config_handler_.config_->http.server_list[2],
									&config_handler_.config_->http.server_list[2].location_list[0],
									request,
									cli_addr3));
	// allow cli_addr4
	request.uri = "/hello";
	EXPECT_TRUE(config_handler_.allowRequest(config_handler_.config_->http.server_list[2],
									&config_handler_.config_->http.server_list[2].location_list[1],
									request,
									cli_addr4));
}

/*TEST_F(ConfigHandlerTest, searchFile)
{
	struct TiedServer	tied_server_;

	// 絶対パス取得
	std::string	file_path = "../../";
	char		absolute_path[MAXPATHLEN];
	std::string	absolutepath;

	// 絶対pathを取得
	if (realpath(file_path.c_str(), absolute_path) == NULL)
	{
		std::cerr << file_path << " is not found." << std::endl;
		exit(EXIT_FAILURE);
	}

	//absolutepath = ~/webserv/html
	absolutepath = static_cast<std::string>(absolute_path);

	HttpRequest	request;

	tied_server_.servers_.clear();
	tied_server_.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	request.headers["Host"] = "first_server";
	// indexのみ設定されているケース
	request.uri = "/";
	EXPECT_EQ(absolutepath + "/index.html",
			config_handler_.searchFile(tied_server_,
									request));
	// indexとtry_filesが設定されているケース
	request.uri = "/test/";
	EXPECT_EQ(absolutepath + "/test/try.html",
			config_handler_.searchFile(tied_server_,
									request));

	tied_server_.servers_.clear();
	tied_server_.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	request.headers["Host"] = "second_server";
	// index,try_files.returnが設定されているケース
	request.uri = "/test";
	EXPECT_EQ(absolutepath + "/404.html",
			config_handler_.searchFile(tied_server_,
									request));

	tied_server_.servers_.clear();
	tied_server_.servers_.push_back(&config_handler_.config_->http.server_list[2]);
	request.headers["Host"] = "third_server";
	// location内でrootが変わっているケース
	request.uri = "/";
	EXPECT_EQ(absolutepath + "/change/change_root.html",
			config_handler_.searchFile(tied_server_,
									request));
	// aliasでrootが変わるケース
	request.uri = "/test";
	EXPECT_EQ(absolutepath + "/alias/alias.html",
			config_handler_.searchFile(tied_server_,
									request));
}
*/

TEST_F(ConfigHandlerTest, searchKeepaliveTimeout)
{
	struct TiedServer	tied_server_1("127.0.0.1", 8001);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	// http set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.keepalive_timeout.getTime(),
			config_handler_.searchKeepaliveTimeout(tied_server_1,
											"first_server",
											"/")));
	// location set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.server_list[0].location_list[1].keepalive_timeout.getTime(),
			config_handler_.searchKeepaliveTimeout(tied_server_1,
											"first_server",
											"/hello/")));

	struct TiedServer	tied_server_2("127.0.0.2", 8002);
	tied_server_2.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	// server set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.server_list[1].keepalive_timeout.getTime(),
			config_handler_.searchKeepaliveTimeout(tied_server_2,
											"second_server",
											"/")));
}

TEST_F(ConfigHandlerTest, searchSendTimeout)
{
	struct TiedServer	tied_server_1("127.0.0.1", 8001);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	// default time
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.send_timeout.getTime(),
			config_handler_.searchSendTimeout(tied_server_1,
										"first_server",
										"/")));
	// location set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.server_list[0].location_list[1].send_timeout.getTime(),
			config_handler_.searchSendTimeout(tied_server_1,
										"first_server",
										"/hello/")));

	struct TiedServer	tied_server_2("127.0.0.2", 8002);
	tied_server_2.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	// server set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.server_list[1].send_timeout.getTime(),
			config_handler_.searchSendTimeout(tied_server_2,
										"second_server",
										"/")));
}

TEST_F(ConfigHandlerTest, searchUseridExpires)
{
	struct TiedServer	tied_server_1("127.0.0.1", 8001);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	// http set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.userid_expires.getTime(),
			config_handler_.searchUseridExpires(tied_server_1,
										"first_server",
										"/")));
	// location set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.server_list[0].location_list[1].userid_expires.getTime(),
			config_handler_.searchUseridExpires(tied_server_1,
										"first_server",
										"/hello/")));

	struct TiedServer	tied_server_2("127.0.0.2", 8002);
	tied_server_2.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	// server set
	EXPECT_TRUE(test::sameTime(config_handler_.config_->http.server_list[1].userid_expires.getTime(),
			config_handler_.searchUseridExpires(tied_server_2,
										"second_server",
										"/")));
}


TEST_F(ConfigHandlerTest, writeAcsLog)
{
	std::string	file_path = "../../";
	char		absolute_path[MAXPATHLEN];
	std::string	absolutepath;

	// 絶対pathを取得
	if (realpath(file_path.c_str(), absolute_path) == NULL)
	{
		std::cerr << file_path << " is not found." << std::endl;
		exit(EXIT_FAILURE);
	}

	//absolutepath = ~/webserv
	absolutepath = static_cast<std::string>(absolute_path);
	std::string	file;
	std::string	msg;


	struct TiedServer	tied_server_1("127.0.0.1", 8001);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	// offというファイルに書き込むのではなく、どこにも書き込まない
	file = absolutepath + "/off";
	msg = "aiueo";
	config_handler_.writeAcsLog(tied_server_1,
							"first_server",
							"/",
							msg);
	EXPECT_FALSE(test::WRITE_ACCURATE(file, msg));

	// ロケーションブロックで指定されたところへ出力
	file = absolutepath + "/logs/location.log";
	msg = "kakikukeko";
	config_handler_.writeAcsLog(tied_server_1,
							"first_server",
							"/hello/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));

	// 親ブロックで指定されたファイルに出力
	file = absolutepath + "/logs/server.log";
	msg = "sashisuseso";
	config_handler_.writeAcsLog(tied_server_1,
							"first_server",
							"/goodnight/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));


	struct TiedServer	tied_server_2("127.0.0.2", 8002);
	tied_server_2.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	// 親の親ブロックで指定されたファイルに出力
	file = absolutepath + "/logs/http.log";
	msg = "tachitsuteto";
	config_handler_.writeAcsLog(tied_server_2,
							"second_server",
							"/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));
}

TEST_F(ConfigHandlerTest, writeErrLog)
{
	std::string	file_path = "../../";
	char		absolute_path[MAXPATHLEN];
	std::string	absolutepath;

	// 絶対pathを取得
	if (realpath(file_path.c_str(), absolute_path) == NULL)
	{
		std::cerr << file_path << " is not found." << std::endl;
		exit(EXIT_FAILURE);
	}

	//absolutepath = ~/webserv
	absolutepath = static_cast<std::string>(absolute_path);
	std::string	file;
	std::string	msg;


	struct TiedServer	tied_server_1("127.0.0.1", 8001);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	// ログオフ
	file = "/dev/null";
	msg = "aiueo";
	config_handler_.writeAcsLog( tied_server_1,
							"first_server",
							"/",
							msg);
	EXPECT_FALSE(test::WRITE_ACCURATE(file, msg));

	// location.logに出力
	file = absolutepath + "/logs/location.log";
	msg = "kakikukeko";
	config_handler_.writeAcsLog(tied_server_1,
							"first_server",
							"/hello/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));


	struct TiedServer	tied_server_2("127.0.0.2", 8002);
	tied_server_2.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	// default fileに出力
	file = absolutepath + "/logs/error.log";
	msg = "sashisuseso";
	config_handler_.writeAcsLog(tied_server_2,
							"second_server",
							"/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));

}

TEST_F(ConfigHandlerTest, createTiedServer)
{
	// 3つのサーバーが該当する場合。
	struct TiedServer	tied_server_1("127.0.0.1", 8001);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[0]);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[2]);
	tied_server_1.servers_.push_back(&config_handler_.config_->http.server_list[3]);
	EXPECT_TRUE(test::sameTiedServer(tied_server_1, config_handler_.createTiedServer("127.0.0.1", 8001)));

	// １つのサーバーが該当する場合
	struct TiedServer	tied_server_2("127.0.0.2", 8002);
	tied_server_2.servers_.push_back(&config_handler_.config_->http.server_list[1]);
	EXPECT_TRUE(test::sameTiedServer(tied_server_2, config_handler_.createTiedServer("127.0.0.2", 8002)));

	// 1つのサーバーが該当する場合
	struct TiedServer	tied_server_3("127.0.0.3", 8003);
	tied_server_3.servers_.push_back(&config_handler_.config_->http.server_list[4]);
	EXPECT_TRUE(test::sameTiedServer(tied_server_3, config_handler_.createTiedServer("127.0.0.3", 8003)));
}

