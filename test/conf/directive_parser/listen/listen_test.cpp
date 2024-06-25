#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const std::vector<config::Listen> &listen_list, const std::vector<std::string> &address_list,
                const std::vector<unsigned int> &port_list, const std::vector<bool> &default_server_list) {
  int li = 0;  // listen index
  std::for_each(listen_list.begin(), listen_list.end(),
                [&li, &address_list, &port_list, &default_server_list](config::Listen listen) {
                  // test address
                  EXPECT_EQ(listen.getAddress(), address_list[li]);

                  // test port
                  EXPECT_EQ(listen.getport(), port_list[li]);

                  // test uri
                  EXPECT_EQ(listen.getIsDefaultServer(), default_server_list[li]);
                  ++li;
                });
}
}  // namespace test

const std::string kListen = "listen";

TEST(ListenTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/listen/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // server
  test::test_value(
      server_list[0].listen_list_,
      {"1.1.1.1", "127.0.0.1", "2.2.2.2", "3.3.3.3", "127.0.0.1", "127.0.0.1", "127.0.0.2", "127.0.0.1"},
      {80, 1, 2000, 80, 2400, 80, 0, 65535}, {false, false, false, true, true, false, false, true});
  test::test_directives_set(server_list[0].directives_set_, kListen, true);
}

TEST(ListenTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(server_list[0].directives_set_, kListen, false);
}
