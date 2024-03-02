#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "InitLogFd.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/param.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// 各テストで使うconfigをセットするクラス
class InitLogTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path;
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();
		if (static_cast<std::string>(test_info->name()) == "initAcsLogFds_Success") {
			file_path = "test/conf/log_test_files/initAcsLogFds_success.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "initAcsLogFds_Fail") {
			file_path = "test/conf/log_test_files/initAcsLogFds_fail.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "initErrLogFds_Success") {
			file_path = "test/conf/log_test_files/initErrLogFds_success.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "initErrLogFds_Fail") {
			file_path = "test/conf/log_test_files/initErrLogFds_fail.conf";
		}
		else {
			this->config_ = new config::Main();
			GTEST_SKIP();
		}

		this->config_ = new config::Main();
		config::Lexer	lexer(file_path);
		lexer.tokenize();

		config::Parser	parser(*this->config_, lexer.getTokens(), file_path);
		parser.parse();
	}


	void TearDown() override {
        // テストケースのクリーンアップ処理
		delete config_;
    }

	// テストに使うオブジェクト
	config::Main	*config_;
};

TEST_F(InitLogTest, initAcsLogFds_Success)
{
	int	ret;

	ret = config::initAcsLogFds(*config_);

	// 関数の結果が正しいか
	EXPECT_TRUE(ret);

	// fdの数が正確かどうか
	// http contezt
	EXPECT_EQ(config_->http.access_fd_list.size(), 0);
	// server context
	EXPECT_EQ(config_->http.server_list[0].access_fd_list.size(), 3);
	EXPECT_EQ(config_->http.server_list[0].access_fd_list.size(), 0);
	EXPECT_EQ(config_->http.server_list[0].access_fd_list.size(), 0);
	// location context
	for (size_t i = 0; i < config_->http.server_list.size(); i++)
	{
		for (size_t j = 0; i < config_->http.server_list[i].location_list.size(); j++)
		{
			EXPECT_EQ(config_->http.server_list[i].location_list[j].access_fd_list.size(), 2);
		}
	}
}

TEST_F(InitLogTest, initAcsLogFds_Fail)
{
	int	ret;

	ret = config::initAcsLogFds(*config_);

	// 関数の結果が正しいか
	EXPECT_FALSE(ret);
}

TEST_F(InitLogTest, initErrLogFds_Success)
{
	int	ret;

	ret = config::initErrLogFds(*config_);

	// 関数の結果が正しいか
	EXPECT_TRUE(ret);

	// fdの数が正確かどうか
	// main context
	EXPECT_EQ(config_->error_fd_list.size(), 1);
	// http contezt
	EXPECT_EQ(config_->http.error_fd_list.size(), 2);
	// server context
	EXPECT_EQ(config_->http.server_list[0].error_fd_list.size(), 3);
	EXPECT_EQ(config_->http.server_list[1].error_fd_list.size(), 0);
	EXPECT_EQ(config_->http.server_list[2].error_fd_list.size(), 0);
	// location context
	for (size_t i = 0; i < config_->http.server_list.size(); i++)
	{
		for (size_t j = 0; i < config_->http.server_list[i].location_list.size(); j++)
		{
			EXPECT_EQ(config_->http.server_list[i].location_list[j].error_fd_list.size(), 2);
		}
	}
}

TEST_F(InitLogTest, initErrLogFds_Fail)
{
	int	ret;

	ret = config::initErrLogFds(*config_);

	// 関数の結果が正しいか
	EXPECT_FALSE(ret);
}

