#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kSendTimeout = "send_timeout";

namespace test {
void test_st_value(const std::vector<config::Location> &list, const std::vector<unsigned long> &expect) {
  int i = 0;
  std::for_each(list.begin(), list.end(), [&i, &expect](config::Location location) {
    EXPECT_EQ(location.send_timeout.getTime().time_in_ms_, expect[i]);
    test::test_directives_set(location.directives_set, kSendTimeout, true);
    ++i;
  });
}
}  // namespace test

TEST(sendTimeoutTest, max) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/send_timeout/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  const long kLongMax = std::numeric_limits<long>::max();

  // http
  EXPECT_EQ(http.send_timeout.getTime().time_in_ms_, kLongMax);
  test::test_directives_set(http.directives_set, kSendTimeout, true);

  // server
  EXPECT_EQ(server_list[0].send_timeout.getTime().time_in_ms_, 9223372036854775 * config::Time::seconds);
  test::test_directives_set(server_list[0].directives_set, kSendTimeout, true);

  // location
  test::test_st_value(server_list[0].location_list, {
                                                             9223372036854775 * config::Time::seconds,
                                                             153722867280912 * config::Time::minutes,
                                                             2562047788015 * config::Time::hours,
                                                             106751991167 * config::Time::days,
                                                         });
}

TEST(SendTimeoutTest, zero) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/send_timeout/2.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  // http
  EXPECT_EQ(http.send_timeout.getTime().time_in_ms_, 0);
  test::test_directives_set(http.directives_set, kSendTimeout, true);

  // server
  EXPECT_EQ(server_list[0].send_timeout.getTime().time_in_ms_, 0);
  test::test_directives_set(server_list[0].directives_set, kSendTimeout, true);

  // location
  test::test_st_value(server_list[0].location_list, {0, 0, 0, 0});
}

TEST(SendTimeoutTest, random) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/send_timeout/3.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  // http
  EXPECT_EQ(http.send_timeout.getTime().time_in_ms_, 2);
  test::test_directives_set(http.directives_set, kSendTimeout, true);

  // server
  EXPECT_EQ(server_list[0].send_timeout.getTime().time_in_ms_, 2000);
  test::test_directives_set(server_list[0].directives_set, kSendTimeout, true);

  // location
  test::test_st_value(server_list[0].location_list,
                      {2 * 1000, 2 * 1000 * 60, 2 * 1000 * 60 * 60, 2 * 1000 * 60 * 60 * 24});
}

TEST(SendTimeoutTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;
  (void)server_list;

  test::test_directives_set(http.directives_set, kSendTimeout, false);
}
