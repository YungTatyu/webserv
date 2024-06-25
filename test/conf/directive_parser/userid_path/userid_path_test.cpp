#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kUseridPath = "userid_path";

TEST(useridPathTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/userid_path/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_EQ(http.userid_path.getPath(), "/path");
  test::test_directives_set(http.directives_set_, kUseridPath, true);

  // server
  EXPECT_EQ(server_list[0].userid_path.getPath(), "123");
  test::test_directives_set(server_list[0].directives_set_, kUseridPath, true);

  // location
  EXPECT_EQ(server_list[0].location_list_[0].userid_path.getPath(), "/tmp/dir");
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUseridPath, true);
}

TEST(useridPathTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kUseridPath, false);
  test::test_directives_set(server_list[0].directives_set_, kUseridPath, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUseridPath, false);
}
