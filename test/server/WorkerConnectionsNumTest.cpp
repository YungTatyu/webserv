#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <gtest/internal/gtest-port.h>
#include "conf.hpp"
#include "WebServer.hpp"
#include <iostream>

class WorkerConnectionsNumTest : public ::testing::Test {
protected:
    // 各TESTの前に呼び出されるセットアップメソッド
	void SetUp() override {
		std::string		file_path;
		const testing::TestInfo*	test_info = testing::UnitTest::GetInstance()->current_test_info();
		if (static_cast<std::string>(test_info->name()) == "OK") {
			file_path = "test/server/WorkerConnectionsNumTestFiles/OK.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "ERROR_MORE_THAN_MAX") {
			file_path = "test/server/WorkerConnectionsNumTestFiles/ERROR_MORE_THAN_MAX.conf";
		}
		else if (static_cast<std::string>(test_info->name()) == "ERROR_EQ_MAX") {
			file_path = "test/server/WorkerConnectionsNumTestFiles/ERROR_EQ_MAX.conf";
		}
		else {
			this->config_ = new config::Main();
			GTEST_SKIP();
		}

		this->config_ = config::initConfig(file_path);
	}

	void TearDown() override {
		// テストケースのクリーンアップ処理
	}

	config::Main*	config_;
};

TEST_F(WorkerConnectionsNumTest, OK)
{
	// test_file: "test/server/WorkerConnectionsNumTestFiles/OK.conf";
	testing::internal::CaptureStdout();
	ASSERT_NO_THROW(WebServer(this->config_));
	testing::internal::GetCapturedStdout();
}

TEST_F(WorkerConnectionsNumTest, ERROR_EQ_MAX)
{
	// test_file: "test/server/WorkerConnectionsNumTestFiles/ERROR_EQ_MAX.conf";
	testing::internal::CaptureStdout();
	try
	{
		WebServer	server(this->config_);
		FAIL();
	}
	catch( const std::exception& err )
	{
		ASSERT_STREQ("webserv: [emerg] 5 worker_connections are not enough for 5 listening sockets", err.what());
	}
	testing::internal::GetCapturedStdout();
}

TEST_F(WorkerConnectionsNumTest, ERROR_MORE_THAN_MAX)
{
	// test_file: "test/server/WorkerConnectionsNumTestFiles/ERROR_MORE_THAN_MAX.conf";
	testing::internal::CaptureStdout();
	try
	{
		WebServer	server(this->config_);
		FAIL();
	}
	catch( const std::exception& err )
	{
		ASSERT_STREQ("webserv: [emerg] 5 worker_connections are not enough for 6 listening sockets", err.what());
	}
	testing::internal::GetCapturedStdout();
}
