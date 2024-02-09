#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ServerConfig.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

class ServerConfigTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path = "test/server/conf_files/server_config_test.conf";
		config::Main	*config = new config::Main();
		config::Lexer	lexer(file_path);
		lexer.tokenize();

		config::Parser	parser(*config, lexer.getTokens(), file_path);
		parser.parse();

		server_config_.loadConfiguration(config);
	}

	// テストに使う共有のオブジェクト
	ServerConfig	server_config_;
};

TEST_F(ServerConfigTest, getServer1)
{
	config::Server&	server;

	server = server_config_.getServer("first_server", "127.0.0.1", 8001);

	EXPECT_EQ(server_config_.config_->http.server_list[0], server);
}

TEST_F(ServerConfigTest, getServer2)
{
	config::Server&	server;

	server = server_config_.getServer("second_server", "127.0.0.2", 8002);

	EXPECT_EQ(server_config_.config_->http.server_list[1], server);
}

TEST_F(ServerConfigTest, getServer3)
{
	config::Server&	server;

	server = server_config_.getServer("third_server", "127.0.0.3", 8003);

	EXPECT_EQ(server_config_.config_->http.server_list[2], server);
}

TEST_F(ServerConfigTest, getServer4)
{
	config::Server&	server;

	server = server_config_.getServer("default_server", "127.0.0.10", 8080);

	// server[1]がデフォルトサーバー
	EXPECT_EQ(server_config_.config_->http.server_list[1], server);
}

TEST_F(ServerConfigTest, getLocatio1)
{
	config::Location&	location;

	location = server_config_.getLocation(server_config_.http.server[0], "hello");

	EXPECT_EQ(server_config_.config_->http.server_list[0].location_list[1], location);
}

