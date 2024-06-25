#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kReceiveTimeout = "receive_timeout";

namespace test {
void test_rtvalue(const std::vector<config::Location> &list, const std::vector<unsigned long> &expect) {
  int i = 0;
  std::for_each(list.begin(), list.end(), [&i, &expect](config::Location location) {
    EXPECT_EQ(location.receive_timeout_.getTime().time_in_ms_, expect[i]);
    test::test_directives_set(location.directives_set_, kReceiveTimeout, true);
    ++i;
  });
}
}  // namespace test

TEST(ReceiveTimeoutTest, max) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/receive_timeout/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  const long kLongMax = std::numeric_limits<long>::max();

  // http
  EXPECT_EQ(http.receive_timeout_.getTime().time_in_ms_, kLongMax);
  test::test_directives_set(http.directives_set_, kReceiveTimeout, true);

  // server
  EXPECT_EQ(server_list[0].receive_timeout_.getTime().time_in_ms_, 9223372036854775 * config::Time::seconds);
  test::test_directives_set(server_list[0].directives_set_, kReceiveTimeout, true);

  // location
  test::test_rtvalue(server_list[0].location_list_, {
                                                        9223372036854775 * config::Time::seconds,
                                                        153722867280912 * config::Time::minutes,
                                                        2562047788015 * config::Time::hours,
                                                        106751991167 * config::Time::days,
                                                    });
}

TEST(ReceiveTimeoutTest, zero) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/receive_timeout/2.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_EQ(http.receive_timeout_.getTime().time_in_ms_, 0);
  test::test_directives_set(http.directives_set_, kReceiveTimeout, true);

  // server
  EXPECT_EQ(server_list[0].receive_timeout_.getTime().time_in_ms_, 0);
  test::test_directives_set(server_list[0].directives_set_, kReceiveTimeout, true);

  // location
  test::test_rtvalue(server_list[0].location_list_, {0, 0, 0, 0});
}

TEST(ReceiveTimeoutTest, random) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/receive_timeout/3.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_EQ(http.receive_timeout_.getTime().time_in_ms_, 1);
  test::test_directives_set(http.directives_set_, kReceiveTimeout, true);

  // server
  EXPECT_EQ(server_list[0].receive_timeout_.getTime().time_in_ms_, 1000);
  test::test_directives_set(server_list[0].directives_set_, kReceiveTimeout, true);

  // location
  test::test_rtvalue(server_list[0].location_list_, {1000, 1000 * 60, 1000 * 60 * 60, 1000 * 60 * 60 * 24});
}

TEST(ReceiveTimeoutTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kReceiveTimeout, false);
}
