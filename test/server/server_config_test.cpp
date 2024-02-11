#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ServerConfig.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/param.h>

class ServerConfigTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path;
		if (IsSubTest("allowRequest")) {
			file_path = "test/server/conf_files/allowRequest_test.conf";
		}
		else if (IsSubTest("getFile")) {
			file_path = "test/server/conf_files/getFile_test.conf";
		}
		else if (IsSubTest("getKeepaliveTimeout")) {
			file_path = "test/server/conf_files/getKeepaliveTimeout_test.conf";
		}
		else if (IsSubTest("getSendTimeout")) {
			file_path = "test/server/conf_files/getSendTimeout_test.conf";
		}
		else if (IsSubTest("getUseridExpires")) {
			file_path = "test/server/conf_files/getUseridExpires_test.conf";
		}
		else if (IsSubTest("getClientMaxBodySize")) {
			file_path = "test/server/conf_files/getClientMaxBodySize_test.conf";
		}
		else if (IsSubTest("writeAcsLog")) {
			file_path = "test/server/conf_files/writeAcsLog_test.conf";
		}
		else if (IsSubTest("writeErrLog")) {
			file_path = "test/server/conf_files/writeErrLog_test.conf";
		}
		else {
			SKIP();
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

	// テストに使う共有のオブジェクト
	ServerConfig	server_config_;
};

// allowRequestの引数プラスでclient自身のアドレス必要かも
TEST_F(ServerConfigTest, allowRequest)
{
	in_addr_t	cli_addr1 = 3232235521; // 192.168.0.1の３２bit整数表現
	in_addr_t	cli_addr2 = 3232235522; // 192.168.0.2の３２bit整数表現
	in_addr_t	cli_addr3 = 3232235523; // 192.168.0.3の３２bit整数表現
	in_addr_t	cli_addr4 = 3232235524; // 192.168.0.4の３２bit整数表現
	EXPECT_TRUE(server_config_.allowRequest("first_server",
											"127.0.0.1",
											8001,
											"",
											cli_addr1));
	EXPECT_FALSE(server_config_.allowRequest("first_server",
											"127.0.0.1",
											8001,
											"hello/"));
	EXPECT_TRUE(server_config_.allowRequest("second_server",
											"127.0.0.2",
											8002,
											"",
											cli_addr1));
	EXPECT_FALSE(server_config_.allowRequest("second_server",
											"127.0.0.2",
											8002,
											"hello",
											cli_addr2));
	EXPECT_FALSE(server_config_.allowRequest("third_server",
											"192.168.0.3",
											8003,
											"hello",
											cli_addr3));
	EXPECT_TRUE(server_config_.allowRequest("third_server",
											"127.0.0.3",
											8003,
											"hello",
											cli_addr4));
}

TEST_F(ServerConfigTest, getFile)
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

	// indexのみ設定されているケース
	EXPECT_EQ(absolutepath + "/index.html",
			server_config_.getFile("first_server",
									"127.0.0.1",
									8001,
									"/"));
	// indexとtry_filesが設定されているケース
	EXPECT_EQ(absolutepath + "/test/try.html",
			server_config_.getFile("first_server",
									"127.0.0.1",
									8001,
									"/test/"));
	// index,try_files.returnが設定されているケース
	EXPECT_EQ(absolutepath + "/404.html",
			server_config_.getFile("second_server",
									"127.0.0.2",
									8002,
									"/test"));
	// location内でrootが変わっているケース
	EXPECT_EQ(absolutepath + "/change/change_root.html",
			server_config_.getFile("third_server",
									"127.0.0.3",
									8003,
									"/"));
	// aliasでrootが変わるケース
	EXPECT_EQ(absolutepath + "/alias/alias.html",
			server_config_.getFile("third_server",
									"127.0.0.3",
									8003,
									"/test"));
}

TEST_F(ServerConfigTest, getKeepaliveTimeout)
{
	EXPECT_EQ(, server_config_.getKeepaliveTimeout());
	EXPECT_EQ(, server_config_.getKeepaliveTimeout());
	EXPECT_EQ(, server_config_.getKeepaliveTimeout());
	EXPECT_EQ(, server_config_.getKeepaliveTimeout());
	EXPECT_EQ(, server_config_.getKeepaliveTimeout());
	EXPECT_EQ(, server_config_.getKeepaliveTimeout());
}

TEST_F(ServerConfigTest, getSendTimeout)
{
	EXPECT_EQ(, server_config_.getSendTimeout());
	EXPECT_EQ(, server_config_.getSendTimeout());
	EXPECT_EQ(, server_config_.getSendTimeout());
	EXPECT_EQ(, server_config_.getSendTimeout());
	EXPECT_EQ(, server_config_.getSendTimeout());
	EXPECT_EQ(, server_config_.getSendTimeout());
}

TEST_F(ServerConfigTest, getUseridExpires)
{
	EXPECT_EQ(, server_config_.getUseridExpires());
	EXPECT_EQ(, server_config_.getUseridExpires());
	EXPECT_EQ(, server_config_.getUseridExpires());
	EXPECT_EQ(, server_config_.getUseridExpires());
	EXPECT_EQ(, server_config_.getUseridExpires());
	EXPECT_EQ(, server_config_.getUseridExpires());
}

TEST_F(ServerConfigTest, getClientMaxBodySize())
{
	EXPECT_EQ(, server_client_.getClientMaxBodySize());
	EXPECT_EQ(, server_client_.getClientMaxBodySize());
	EXPECT_EQ(, server_client_.getClientMaxBodySize());
	EXPECT_EQ(, server_client_.getClientMaxBodySize());
	EXPECT_EQ(, server_client_.getClientMaxBodySize());
	EXPECT_EQ(, server_client_.getClientMaxBodySize());
}

TEST_F(ServerConfigTest, writeAcsLog)
{
	server_config_.writeAcsLog();

	WRITE_ACCURATE();
}

TEST_F(ServerConfigTest, writeErrLog)
{
	server_config_.writeErrLog();

	WRITE_ACCURATE();
}

