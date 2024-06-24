#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const std::vector<config::Server> &list, const std::vector<bool> &expect) {
  int i = 0;
  std::for_each(list.begin(), list.end(), [&i, &expect](config::Server server) {
    EXPECT_EQ(server.autoindex_.getIsAutoindexOn(), expect[i]);
    ++i;
  });
}
}  // namespace test

const std::string kAutoindex = "autoindex";

TEST(autoindexTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/autoindex/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_EQ(http.autoindex_.getIsAutoindexOn(), true);
  test::test_directives_set(http.directives_set_, kAutoindex, true);

  // server
  test::test_value(server_list, {false, true});
  test::test_directives_set(server_list[0].directives_set_, kAutoindex, true);
  test::test_directives_set(server_list[1].directives_set_, kAutoindex, true);

  // location
  EXPECT_EQ(server_list[0].location_list_[0].autoindex_.getIsAutoindexOn(), true);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kAutoindex, true);
}

TEST(autoindexTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kAutoindex, false);
  test::test_directives_set(server_list[0].directives_set_, kAutoindex, false);
  test::test_directives_set(server_list[1].directives_set_, kAutoindex, false);
  test::test_directives_set(server_list[2].directives_set_, kAutoindex, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kAutoindex, false);
}
