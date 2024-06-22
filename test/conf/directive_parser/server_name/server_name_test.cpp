#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const std::set<std::string> &set, const std::vector<std::string> &expects) {
  std::for_each(expects.begin(), expects.end(),
                [&set](std::string expect) { EXPECT_TRUE(set.find(expect) != set.end()); });
}
}  // namespace test

const std::string kServerName = "server_name";

TEST(ServerNameTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/server_name/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  // server
  test::test_value(server_list[0].server_name.getName(), {
                                                                  "tachu",
                                                                  "",
                                                                  "1",
                                                                  "2",
                                                                  "3",
                                                                  "server name",
                                                                  "sn:8000",
                                                              });
  test::test_directives_set(server_list[0].directives_set, kServerName, true);
}

TEST(ServerNameTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  test::test_directives_set(server_list[0].directives_set, kServerName, false);
  test::test_directives_set(server_list[1].directives_set, kServerName, false);
}
