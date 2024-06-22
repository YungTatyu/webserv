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

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  // http
  EXPECT_EQ(http.userid_path.getPath(), "/path");
  test::test_directives_set(http.directives_set, kUseridPath, true);

  // server
  EXPECT_EQ(server_list[0].userid_path.getPath(), "123");
  test::test_directives_set(server_list[0].directives_set, kUseridPath, true);

  // location
  EXPECT_EQ(server_list[0].location_list[0].userid_path.getPath(), "/tmp/dir");
  test::test_directives_set(server_list[0].location_list[0].directives_set, kUseridPath, true);
}

TEST(useridPathTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  test::test_directives_set(http.directives_set, kUseridPath, false);
  test::test_directives_set(server_list[0].directives_set, kUseridPath, false);
  test::test_directives_set(server_list[0].location_list[0].directives_set, kUseridPath, false);
}
