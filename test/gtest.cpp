
#include <gtest/gtest.h>

#include "GlobalTestEnvironment.hpp"

// グローバルテスト環境のインスタンスを生成
::testing::Environment* createGlobalTestEnvironment() {
  static GlobalTestEnvironment* env = new GlobalTestEnvironment();
  return env;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // グローバルなセットアップとクリーンアップを登録
  ::testing::AddGlobalTestEnvironment(createGlobalTestEnvironment());

  return RUN_ALL_TESTS();
}
