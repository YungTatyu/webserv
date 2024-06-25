#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const std::vector<config::Return> &return_list, const std::vector<int> &code_list,
                const std::vector<std::string> &url_list) {
  int i = 0;  // Return index
  std::for_each(return_list.begin(), return_list.end(), [&i, &code_list, &url_list](config::Return return_) {
    // test code
    EXPECT_EQ(return_.getCode(), code_list[i]);

    // test url
    EXPECT_EQ(return_.getUrl(), url_list[i]);

    ++i;
  });
}
}  // namespace test

const std::string kReturn = "return";

TEST(ReturnTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/return/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // location
  test::test_value(server_list[0].location_list_[0].return_list_,
                   {300, -1, -1, 999, 998, 0, 0, 301, 302, 303, 307, 308},
                   {"url", "https://www.tachu.com", "http://", "url999", "1000", "", "", "https://",
                    "http://a", "http://b", "http://c", "http://d"});
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kReturn, true);
}

TEST(ReturnTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kReturn, false);
}
