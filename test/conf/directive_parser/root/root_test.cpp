#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kRoot = "root";

TEST(RootTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/root/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_EQ(http.root_.getPath(), "");
  test::test_directives_set(http.directives_set_, kRoot, true);

  // server
  EXPECT_EQ(server_list[0].root_.getPath(), "123");
  test::test_directives_set(server_list[0].directives_set_, kRoot, true);

  // location
  EXPECT_EQ(server_list[0].location_list_[0].root_.getPath(), "/tmp");
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kRoot, true);
}

TEST(RootTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kRoot, false);
  test::test_directives_set(server_list[0].directives_set_, kRoot, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kRoot, false);
}
