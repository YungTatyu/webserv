#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kUseridService = "userid_service";

TEST(useridServiceTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/userid_service/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  EXPECT_EQ(http.userid_service.getUseridService(), 9223372036854775807);
  test::test_directives_set(http.directives_set_, kUseridService, true);

  // server
  EXPECT_EQ(server_list[0].userid_service.getUseridService(), 0);
  test::test_directives_set(server_list[0].directives_set_, kUseridService, true);

  // location
  EXPECT_EQ(server_list[0].location_list_[0].userid_service.getUseridService(), 9223372036854775806);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUseridService, true);
}

TEST(useridServiceTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kUseridService, false);
  test::test_directives_set(server_list[0].directives_set_, kUseridService, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kUseridService, false);
}
