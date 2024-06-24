#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const std::vector<config::Index> &list, const std::vector<std::string> &expect) {
  int i = 0;
  std::for_each(list.begin(), list.end(), [&i, &expect](config::Index index) {
    EXPECT_EQ(index.getFile(), expect[i]);
    ++i;
  });
}
}  // namespace test

const std::string kIndex = "index";

TEST(indexTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/index/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // http
  test::test_value(http.index_list_, {"1.html", "2.html", "3.html", "1", "2", "3", "4", "5"});
  test::test_directives_set(http.directives_set_, kIndex, true);

  // server
  test::test_value(server_list[0].index_list_, {"index.html", "server1", "server2"});
  test::test_directives_set(server_list[0].directives_set_, kIndex, true);

  // location
  test::test_value(
      server_list[0].location_list_[0].index_list_,
      {"location1", "location2", "location3", "index1.html", "index2.html", "index3.html", "index4.html"});
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kIndex, true);
}

TEST(indexTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(server_list[0].directives_set_, kIndex, false);
  test::test_directives_set(server_list[1].directives_set_, kIndex, false);
  test::test_directives_set(server_list[2].directives_set_, kIndex, false);
}
