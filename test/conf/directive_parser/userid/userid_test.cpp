#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kUserid = "userid";

TEST(useridTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/userid/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_TRUE(http.userid.getIsUseridOn());
  test::test_directives_set(http.directives_set_, kUserid, true);

  // server
  EXPECT_FALSE(http.server_list_[0].userid.getIsUseridOn());
  test::test_directives_set(server_list[0].directives_set_, kUserid, true);

  // location
  EXPECT_TRUE(server_list[0].location_list_[0].userid.getIsUseridOn());
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUserid, true);
}

TEST(useridTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kUserid, false);
  test::test_directives_set(server_list[0].directives_set_, kUserid, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUserid, false);
}
