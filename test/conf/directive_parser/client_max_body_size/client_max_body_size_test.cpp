#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kClientMaxBodySize = "client_max_body_size";
const long kLongMax = std::numeric_limits<long>::max();

TEST(clientMaxBodySizeTest, longMax) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/client_max_body_size/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, kLongMax);
  test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}

TEST(clientMaxBodySizeTest, longMaxKilo) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/client_max_body_size/3.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, 9007199254740991 * config::Size::kilobytes);
  test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}

TEST(clientMaxBodySizeTest, longMaxMega) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/client_max_body_size/4.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, 8796093022207 * config::Size::megabytes);
  test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}

TEST(clientMaxBodySizeTest, zero) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/client_max_body_size/2.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, 0);
  test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}

TEST(clientMaxBodySizeTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  test::test_directives_set(http.directives_set, kClientMaxBodySize, false);
}
