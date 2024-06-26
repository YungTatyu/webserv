#include <gtest/gtest.h>
#include <stdlib.h>
#include <sys/param.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Main.hpp"
#include "Parser.hpp"
#include "conf.hpp"
#include "directives_test.hpp"
#include "utils.hpp"

namespace test {
void test_value(const std::vector<config::ErrorLog> &list, const std::vector<std::string> &expect) {
  int i = 0;
  std::for_each(list.begin(), list.end(), [&i, &expect](config::ErrorLog error_log) {
    EXPECT_EQ(error_log.getFile(), expect[i]);
    ++i;
  });
}
}  // namespace test

const std::string kErrorLog = "error_log";

TEST(ErrorLogTest, allContext) {
  const config::Main *config = test::initConfigTest("test/conf/directive_parser/error_log/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // main
  test::test_value(config->error_log_list_, {"/tmp", "/tmp/path"});
  test::test_directives_set(config->directives_set_, kErrorLog, true);

  // http
  test::test_value(http.error_log_list_, {"/", "/path/"});
  test::test_directives_set(http.directives_set_, kErrorLog, true);

  // server
  test::test_value(http.server_list_[0].error_log_list_, {"path1", "path2", "path3"});
  test::test_directives_set(server_list[0].directives_set_, kErrorLog, true);

  // location
  test::test_value(server_list[0].location_list_[0].error_log_list_, {"/server1", "/server2", "/server3"});
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kErrorLog, true);
}

TEST(ErrorLogTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(config->directives_set_, kErrorLog, false);
  test::test_directives_set(http.directives_set_, kErrorLog, false);
  test::test_directives_set(server_list[0].directives_set_, kErrorLog, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kErrorLog, false);
}
