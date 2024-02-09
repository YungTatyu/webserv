#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Webserver.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

class TestConfig : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path = "test/server/conf_files/server_config_test.conf";
		config::Main	*config = new Main();
		config::Lexer	lexern(file_path);
		lexer.tokenize();

		confif::Parser	parser(*config, lexer.getTokens(), file_path);

		server_config_.loadConfig(config);
	}

	// テストに使う共有のオブジェクト
	ServerConfig	server_config_;
};

TEST(ServerConfigTest, getServer)
{
	config::Server&	server;

	server = server_config_.getServer("first_server", "127.0.0.1", 8001);

	EXPECT_true(server_config_.config_.http.server[0], server);
}

TEST(ServerConfigTest, getServer)
{
	config::Server&	server;

	server = server_config_.getServer("second_server", "127.0.0.2", 8002);

	EXPECT_true(server_config_.config_.http.server[1], server);
}

TEST(ServerConfigTest, getServer)
{
	config::Server&	server;

	server = server_config_.getServer("third_server", "127.0.0.3", 8003);

	EXPECT_true(server_config_.config_.http.server[2], server);
}

TEST(ServerConfigTest, getServer)
{
	config::Server&	server;

	server = server_config_.getServer("default_server", "127.0.0.10", 8080);

	// server[1]がデフォルトサーバー
	EXPECT_true(server_config_.config_.http.server[1], server);
}

TEST(ServerConfigTest, getLocation)
{
	config::Location&	location;

	location = server_config_.getLocation(server_config_.http.server[0], "hello");

	EXPECT_true(server_config_.config_.http.server[0].location[1], location);
}

