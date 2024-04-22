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

  const config::Http &http = config->http;
  const config::Events &events = config->events;
  const std::vector<config::Server> &server_list = http.server_list;

  // http
  EXPECT_EQ(http.root.getPath(), "");
  test::test_directives_set(http.directives_set, kRoot, true);

  // server
  EXPECT_EQ(http.server_list[0].root.getPath(), "123");
  test::test_directives_set(http.server_list[0].directives_set, kRoot, true);

  // location
  EXPECT_EQ(http.server_list[0].location_list[0].root.getPath(), "/tmp");
  test::test_directives_set(http.server_list[0].location_list[0].directives_set, kRoot, true);
}

TEST(RootTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const config::Events &events = config->events;
  const std::vector<config::Server> &server_list = http.server_list;

  test::test_directives_set(http.directives_set, kRoot, false);
  test::test_directives_set(http.server_list[0].directives_set, kRoot, false);
  test::test_directives_set(http.server_list[0].location_list[0].directives_set, kRoot, false);
}
