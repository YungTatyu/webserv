#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kUseridExpires = "userid_expires";

namespace test {
void test_value(const std::vector<config::Location> &list, const std::vector<unsigned long> &expect_times,
                const std::vector<bool> expect_is_userid_expires_on) {
  int i = 0;
  std::for_each(list.begin(), list.end(),
                [&i, &expect_times, &expect_is_userid_expires_on](config::Location location) {
                  EXPECT_EQ(location.userid_expires_.getTime().time_in_ms_, expect_times[i]);
                  EXPECT_EQ(location.userid_expires_.getIsUseridExpiresOn(), expect_is_userid_expires_on[i]);
                  test::test_directives_set(location.directives_set_, kUseridExpires, true);
                  ++i;
                });
}
}  // namespace test

TEST(useridExpiresTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/userid_expires/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  const long kLongMax = std::numeric_limits<long>::max();

  // http
  EXPECT_FALSE(http.userid_expires.getIsUseridExpiresOn());
  test::test_directives_set(http.directives_set_, kUseridExpires, true);

  // server
  EXPECT_TRUE(server_list[0].userid_expires.getIsUseridExpiresOn());
  EXPECT_EQ(server_list[0].userid_expires.getTime().time_in_ms_, kLongMax);
  test::test_directives_set(server_list[0].directives_set_, kUseridExpires, true);

  // location
  test::test_value(server_list[0].location_list_, {9223372036854775 * 1000, 0, 3 * 1000 * 60},
                   {true, true, true});
}

TEST(useridExpiresTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kUseridExpires, false);
  test::test_directives_set(server_list[0].directives_set_, kUseridExpires, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUseridExpires, false);
}
