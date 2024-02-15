#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ServerConfig.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/param.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// 各テストで使うconfigをセットするクラス
class ServerConfigTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path;
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();
		if (test_info->name() == "allowRequest") {
			file_path = "test/server/conf_files/allowRequest_test.conf";
		}
		else if (test_info->name() == "searchFile") {
			file_path = "test/server/conf_files/searchFile_test.conf";
		}
		else if (test_info->name() == "searchKeepaliveTimeout") {
			file_path = "test/server/conf_files/searchKeepaliveTimeout_test.conf";
		}
		else if (test_info->name() == "searchSendTimeout") {
			file_path = "test/server/conf_files/searchSendTimeout_test.conf";
		}
		else if (test_info->name() == "searchUseridExpires") {
			file_path = "test/server/conf_files/searchUseridExpires_test.conf";
		}
		else if (test_info->name() == "writeAcsLog") {
			file_path = "test/server/conf_files/writeAcsLog_test.conf";
		}
		else if (test_info->name() == "writeErrLog") {
			file_path = "test/server/conf_files/writeErrLog_test.conf";
		}
		else {
			GTEST_SKIP();
		}
		config::Main	*config = new config::Main();
		config::Lexer	lexer(file_path);
		lexer.tokenize();

		config::Parser	parser(*config, lexer.getTokens(), file_path);
		parser.parse();

		server_config_.loadConfiguration(config);
	}


	void TearDown() override {
        // テストケースのクリーンアップ処理
		delete server_config_.config_;
    }

	// テストに使うオブジェクト
	ServerConfig	server_config_;
	struct TiedServer	vserver_;
};

namespace test {
// 実際に使うallowRequestはソケットをつくらないと使えないので、ここではソケットの代わりに、アドレスを渡している。
bool	allowRequestIPv4( const struct TiedServer& server_config,
					const std::string& server_name,
					const std::string& uri,
					const in_addr cli_addr );
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
};

// allowRequestの引数プラスでclient自身のアドレス必要かも
TEST_F(ServerConfigTest, allowRequest)
{
	struct in_addr	cli_addr1
	struct in_addr	cli_addr2;
	struct in_addr	cli_addr3;
	struct in_addr	cli_addr4;

	// 初期化
	inet_pton(AF_INET, "192.168.0.1", &cli_addr1);
	inet_pton(AF_INET, "192.168.0.2", &cli_addr2);
	inet_pton(AF_INET, "192.168.0.3", &cli_addr3);
	inet_pton(AF_INET, "192.168.0.4", &cli_addr4);


	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// allow all
	EXPECT_TRUE(test::allowRequestIPv4(vserver_,
									"first_server",
									"/",
									cli_addr1));
	// deny all
	EXPECT_FALSE(test::allowRequestIPv4(vserver_,
									"first_server",
									"/hello/",
									cli_addr1));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// allow cli_addr2
	EXPECT_TRUE(test::allowRequestIPv4(vserver_,
									"second_server",
									"/",
									cli_addr2));
	// deny cli_addr2
	EXPECT_FALSE(test::allowRequestIPv4(vserver_,
									"second_server",
									"/hello",
									cli_addr2));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[2]);
	// deny cli_addr3 
	EXPECT_FALSE(test::allowRequestIPv4(vserver_,
									"third_server",
									"/",
									cli_addr3));
	// allow cli_addr4
	EXPECT_TRUE(test::allowRequestIPv4(vserver_,
									"third_server",
									"/hello",
									cli_addr4));
}

TEST_F(ServerConfigTest, searchFile)
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

	//absolutepath = ~/webserv/html
	absolutepath = static_cast<std::string>(absolute_path);



	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// indexのみ設定されているケース
	EXPECT_EQ(absolutepath + "/index.html",
			server_config_.searchFile(vserver_,
									"first_server",
									"/"));
	// indexとtry_filesが設定されているケース
	EXPECT_EQ(absolutepath + "/test/try.html",
			server_config_.searchFile(vserver_,
									"first_server",
									"/test/"));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// index,try_files.returnが設定されているケース
	EXPECT_EQ(absolutepath + "/404.html",
			server_config_.searchFile(vserver_,
									"second_server",
									"/test"));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[2]);
	// location内でrootが変わっているケース
	EXPECT_EQ(absolutepath + "/change/change_root.html",
			server_config_.searchFile(vserver_,
									"third_server",
									"/"));
	// aliasでrootが変わるケース
	EXPECT_EQ(absolutepath + "/alias/alias.html",
			server_config_.searchFile(vserver_,
									"third_server",
									"/test"));
}

TEST_F(ServerConfigTest, searchKeepaliveTimeout)
{
	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// http set
	EXPECT_EQ(server_config_.config_->http.keepalive_timeout.getTime(),
			server_config_.searchKeepaliveTimeout(vserver_,
											"first_server",
											"/"));
	// location set
	EXPECT_EQ(server_config_.config_->http.server_list[0].location_list[1].keepalive_timeout.getTime(),
			server_config_.searchKeepaliveTimeout(vserver_,
											"first_server",
											"/hello"));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// server set
	EXPECT_EQ(server_config_.config_->http.server_list[1].keepalive_timeout.getTime(),
			server_config_.searchKeepaliveTimeout(vserver_,
											"second_server",
											"/"));
}

TEST_F(ServerConfigTest, searchSendTimeout)
{
	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// default time
	EXPECT_EQ(server_config_.config_->http.send_timeout.getTime(),
			server_config_.searchSendTimeout(vserver_,
										"first_server",
										"/"));
	// location set
	EXPECT_EQ(server_config_.config_->http.server_list[0].location_list[1].send_timeout.getTime(),
			server_config_.searchSendTimeout(vserver_,
										"first_server",
										"/hello"));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// server set
	EXPECT_EQ(server_config_.config_->http.server_list[1].send_timeout.getTime(),
			server_config_.searchSendTimeout(vserver_,
										"second_server",
										"/"));
}

TEST_F(ServerConfigTest, searchUseridExpires)
{
	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// http set
	EXPECT_EQ(server_config_.config_->http.userid_expires.getTime(),
			server_config_.searchUseridExpires(vserver_,
										"first_server",
										"/"));
	// location set
	EXPECT_EQ(server_config_.config_->http.server_list[0].location_list[1].userid_expires.getTime(),
			server_config_.searchUseridExpires(vserver_,
										"first_server",
										"/hello"));

	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// server set
	EXPECT_EQ(server_config_.config_->http.server_list[1].userid_expires.getTime(),
			server_config_.searchUseridExpires(vserver_,
										"second_server",
										"/"));
}


TEST_F(ServerConfigTest, writeAcsLog)
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


	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// offというファイルに書き込むのではなく、どこにも書き込まない
	file = absolutepath + "/off";
	msg = "aiueo";
	server_config_.writeAcsLog(vserver_,
							"first_server",
							"/",
							msg);
	EXPECT_FALSE(test::WRITE_ACCURATE(file, msg));

	// ロケーションブロックで指定されたところへ出力
	file = absolutepath + "/logs/location.log";
	msg = "kakikukeko";
	server_config_.writeAcsLog(vserver_,
							"first_server",
							"/hello",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));

	// 親ブロックで指定されたファイルに出力
	file = absolutepath + "/logs/server.log";
	msg = "sashisuseso";
	server_config_.writeAcsLog(vserver_,
							"first_server",
							"/goodnight",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));


	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// 親の親ブロックで指定されたファイルに出力
	file = absolutepath + "/logs/http.log";
	msg = "tachitsuteto";
	server_config_.writeAcsLog(vserver_,
							"second_server",
							"/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));
}

TEST_F(ServerConfigTest, writeErrLog)
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


	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	// ログオフ
	file = "/dev/null";
	msg = "aiueo";
	server_config_.writeAcsLog( vserver_,
							"first_server",
							"/",
							msg);
	EXPECT_FALSE(test::WRITE_ACCURATE(file, msg));

	// location.logに出力
	file = absolutepath + "/logs/location.log";
	msg = "kakikukeko";
	server_config_.writeAcsLog(vserver_,
							"first_server",
							"/hello",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));


	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	// default fileに出力
	file = absolutepath + "/logs/error.log";
	msg = "sashisuseso";
	server_config_.writeAcsLog(vserver_,
							"second_server",
							"/",
							msg);
	EXPECT_TRUE(test::WRITE_ACCURATE(file, msg));

}

TEST_F(ServerConfigTest, retTiedServer)
{
	// 3つのサーバーが該当する場合。
	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[0]);
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[2]);
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[3]);
	EXPECT_EQ(vserver_, server_config_.retTiedServer("127.0.0.1", 8001));

	// １つのサーバーが該当する場合
	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[1]);
	EXPECT_EQ(vserver_, server_config_.retTiedServer("127.0.0.2", 8002));

	// 1つのサーバーが該当する場合
	vserver_.tied_servers_.clear();
	vserver_.tied_servers_.push_back(&server_config_.config_->http.server_list[4]);
	EXPECT_EQ(vserver_, server_config_.retTiedServer("127.0.0.3", 8003));
}

