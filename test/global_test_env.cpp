
#include <gtest/gtest.h>

#include "WebServer.hpp"

class GlobalTestEnvironment : public ::testing::Environment {
 public:
  ConfigHandler* config_handler_;
  void SetUp() override {
    // std::cout << "GlobalTestEnvironment SetUp" << std::endl;
    this->config_handler_ = &(const_cast<ConfigHandler&>(WebServer::getConfigHandler()));
    config_handler_->loadConfiguration(new config::Main());
  }

  void TearDown() override {
    // std::cout << "GlobalTestEnvironment TearDown" << std::endl;
    // delete this->config_handler_->config_;
  }
};

// グローバルテスト環境のインスタンスを生成
::testing::Environment* GetGlobalTestEnvironment() {
  static GlobalTestEnvironment* env = new GlobalTestEnvironment();
  return env;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // グローバルなセットアップとクリーンアップを登録
  ::testing::AddGlobalTestEnvironment(GetGlobalTestEnvironment());

  return RUN_ALL_TESTS();
}
