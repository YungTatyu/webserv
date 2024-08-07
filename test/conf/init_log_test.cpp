#include <arpa/inet.h>
#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/param.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Lexer.hpp"
#include "LogFd.hpp"
#include "Parser.hpp"
#include "WebServer.hpp"
#include "utils.hpp"

// 各テストで使うconfigをセットするクラス
class InitLogTest : public ::testing::Test {
 protected:
  // 各TESTの前に呼び出されるセットアップメソッド
  void SetUp() override {
    std::string file_path;
    const testing::TestInfo* test_info = testing::UnitTest::GetInstance()->current_test_info();
    if (static_cast<std::string>(test_info->name()) == "initAcsLogFds_Success") {
      file_path = "test/conf/init_log_files/initAcsLogFds_success.conf";
    } else if (static_cast<std::string>(test_info->name()) == "initAcsLogFds_Fail") {
      file_path = "test/conf/init_log_files/initAcsLogFds_fail.conf";
    } else if (static_cast<std::string>(test_info->name()) == "initErrLogFds_Success") {
      file_path = "test/conf/init_log_files/initErrLogFds_success.conf";
    } else if (static_cast<std::string>(test_info->name()) == "initErrLogFds_Fail") {
      file_path = "test/conf/init_log_files/initErrLogFds_fail.conf";
    } else {
      this->config_ = new config::Main();
      GTEST_SKIP();
    }
    this->config_ = new config::Main();
    // 絶対pathを取得
    // std::string	absolute_path;
    // absolute_path = utils::deriveAbsolutePath(file_path);
    // if (absolute_path == "")
    //	GTEST_SKIP();

    // tokenize
    config::Lexer lexer(file_path);
    lexer.tokenize();

    // parse
    config::Parser parser(*this->config_, lexer.getTokens(), file_path);
    if (!parser.parse()) GTEST_SKIP();
  }

  void TearDown() override {
    // テストケースのクリーンアップ処理
    const testing::TestInfo* test_info = testing::UnitTest::GetInstance()->current_test_info();
    if (static_cast<std::string>(test_info->name()) == "initAcsLogFds_Success") {
      unlink("test/conf/init_log_files/test_server1_access.log");
      unlink("test/conf/init_log_files/test_server2_access.log");
      unlink("test/conf/init_log_files/test_server3_access.log");
      unlink("test/conf/init_log_files/test_location1-1_access.log");
      unlink("test/conf/init_log_files/test_location1-2_access.log");
      unlink("test/conf/init_log_files/test_location2-1_access.log");
      unlink("test/conf/init_log_files/test_location2-2_access.log");
      unlink("test/conf/init_log_files/test_location3-1_access.log");
      unlink("test/conf/init_log_files/test_location3-2_access.log");
    } else if (static_cast<std::string>(test_info->name()) == "initAcsLogFds_Fail") {
      unlink("test/conf/init_log_files/test_server1_access.log");
      unlink("test/conf/init_log_files/test_server2_access.log");
      unlink("test/conf/init_log_files/test_server3_access.log");
      unlink("test/conf/init_log_files/test_location1-1_access.log");
      unlink("test/conf/init_log_files/test_location1-2_access.log");
      unlink("test/conf/init_log_files/test_location2-1_access.log");
      unlink("test/conf/init_log_files/test_location2-2_access.log");
    } else if (static_cast<std::string>(test_info->name()) == "initErrLogFds_Success") {
      unlink("off");
      unlink("test/conf/init_log_files/test_error.log");
      unlink("test/conf/init_log_files/test_server1_error.log");
      unlink("test/conf/init_log_files/test_server2_error.log");
      unlink("test/conf/init_log_files/test_server3_error.log");
      unlink("test/conf/init_log_files/test_location1-1_error.log");
      unlink("test/conf/init_log_files/test_location1-2_error.log");
      unlink("test/conf/init_log_files/test_location2-1_error.log");
      unlink("test/conf/init_log_files/test_location2-2_error.log");
      unlink("test/conf/init_log_files/test_location3-1_error.log");
      unlink("test/conf/init_log_files/test_location3-2_error.log");
    } else if (static_cast<std::string>(test_info->name()) == "initErrLogFds_Fail") {
      unlink("off");
      unlink("test/conf/init_log_files/test_error.log");
      unlink("test/conf/init_log_files/test_server1_error.log");
      unlink("test/conf/init_log_files/test_server2_error.log");
      unlink("test/conf/init_log_files/test_server3_error.log");
      unlink("test/conf/init_log_files/test_location1-1_error.log");
      unlink("test/conf/init_log_files/test_location1-2_error.log");
      unlink("test/conf/init_log_files/test_location2-1_error.log");
    }
    config::terminateLogFds(config_);
    delete config_;
  }

  template <typename T>
  int countFdNum(T log_list) {
    int num = 0;
    for (int i = 0; i < log_list.size(); i++) {
      if (log_list[i].getFd() != -1) num++;
    }
    return num;
  }

  // テストに使うオブジェクト
  config::Main* config_;
};

TEST_F(InitLogTest, initAcsLogFds_Success) {
  int ret;

  ret = config::initAcsLogFds(*config_);

  // 関数の結果が正しいか
  ASSERT_TRUE(ret);

  // fdの数が正確かどうか
  // http contezt
  EXPECT_EQ(countFdNum(config_->http_.access_log_list_), 0);
  // server context
  EXPECT_EQ(countFdNum(config_->http_.server_list_[0].access_log_list_), 3);
  EXPECT_EQ(countFdNum(config_->http_.server_list_[1].access_log_list_), 0);
  // location context
  for (size_t i = 0; i < config_->http_.server_list_[0].location_list_.size(); i++) {
    EXPECT_EQ(countFdNum(config_->http_.server_list_[0].location_list_[i].access_log_list_), 2);
  }
}

TEST_F(InitLogTest, initAcsLogFds_Fail) {
  int ret;

  ret = config::initAcsLogFds(*config_);

  // 関数の結果が正しいか
  ASSERT_FALSE(ret);
}

TEST_F(InitLogTest, initErrLogFds_Success) {
  int ret;

  ret = config::initErrLogFds(*config_);

  // 関数の結果が正しいか
  ASSERT_TRUE(ret);

  // fdの数が正確かどうか
  // main context
  EXPECT_EQ(countFdNum(config_->error_log_list_), 1);
  // http contezt
  EXPECT_EQ(countFdNum(config_->http_.error_log_list_), 2);
  // server context
  EXPECT_EQ(countFdNum(config_->http_.server_list_[0].error_log_list_), 3);
  EXPECT_EQ(countFdNum(config_->http_.server_list_[1].error_log_list_), 0);
  // location context
  for (size_t i = 0; i < config_->http_.server_list_[0].location_list_.size(); i++) {
    EXPECT_EQ(countFdNum(config_->http_.server_list_[0].location_list_[i].error_log_list_), 2);
  }
}

TEST_F(InitLogTest, initErrLogFds_Fail) {
  int ret;

  ret = config::initErrLogFds(*config_);

  // 関数の結果が正しいか
  ASSERT_FALSE(ret);
}
