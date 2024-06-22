#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"

namespace test {}  // namespace test

const static std::string HTTP = "http";
const static std::string EVENTS = "events";
const static std::string SERVER = "server";
const static std::string LOCATION = "location";
const static std::string LIMIT_EXCEPT = "limit_except";

TEST(mainParserTest, allContexts) {
  const config::Main *config = config::initConfig("test/conf/conf_files/valid/12.conf");
  const config::Http &http = config->http;
  const config::Events &events = config->events;
  const std::vector<config::Server> &server_list = http.server_list;
  int i;
  (void)events;

  // http
  EXPECT_NE(config->directives_set.find(HTTP), config->directives_set.end());

  // events
  EXPECT_NE(config->directives_set.find(EVENTS), config->directives_set.end());

  // server
  EXPECT_NE(http.directives_set.find(SERVER), http.directives_set.end());

  std::for_each(server_list.begin(), server_list.end() - 1, [](config::Server server) {
    EXPECT_NE(server.directives_set.find(LOCATION), server.directives_set.end());
  });

  EXPECT_EQ(server_list[3].directives_set.find(LOCATION), server_list[3].directives_set.end());
  EXPECT_EQ(server_list.size(), 4);

  // location
  EXPECT_EQ(server_list[0].location_list[0].uri, "path");
  EXPECT_EQ(server_list[1].location_list[0].uri, "");

  std::vector<std::string> expect_str = {
      "path1", "path2", "path3", "path4", "path5", "path6", "path7",
  };
  i = 0;
  std::for_each(expect_str.begin(), expect_str.end(), [&server_list, &i](std::string expect) {
    EXPECT_EQ(server_list[2].location_list[i].uri, expect);
    ++i;
  });

  std::vector<int> expect_size = {1, 1, 7, 0};
  i = 0;
  std::for_each(expect_size.begin(), expect_size.end(), [&server_list, &i](int expect) {
    EXPECT_EQ(server_list[i].location_list.size(), expect);
    ++i;
  });

  // limit except
  EXPECT_NE(server_list[0].location_list[0].directives_set.find(LIMIT_EXCEPT),
            server_list[0].location_list[0].directives_set.end());
  EXPECT_NE(server_list[1].location_list[0].directives_set.find(LIMIT_EXCEPT),
            server_list[1].location_list[0].directives_set.end());
  for (size_t i = 0; i < server_list[2].location_list.size(); i++) {
    EXPECT_EQ(server_list[2].location_list[i].directives_set.find(LIMIT_EXCEPT),
              server_list[2].location_list[i].directives_set.end());
  }

  std::vector<config::REQUEST_METHOD> expect_methods = {
      config::GET,
      config::POST,
      config::DELETE,
      config::HEAD,
  };
  std::for_each(expect_methods.begin(), expect_methods.end(), [&server_list](config::REQUEST_METHOD expect) {
    EXPECT_NE(server_list[0].location_list[0].limit_except.excepted_methods.find(expect),
              server_list[0].location_list[0].limit_except.excepted_methods.end());
  });

  EXPECT_EQ(server_list[1].location_list[0].limit_except.excepted_methods.find(config::GET),
            server_list[1].location_list[0].limit_except.excepted_methods.end());
  EXPECT_NE(server_list[1].location_list[0].limit_except.excepted_methods.find(config::POST),
            server_list[1].location_list[0].limit_except.excepted_methods.end());
  EXPECT_NE(server_list[1].location_list[0].limit_except.excepted_methods.find(config::DELETE),
            server_list[1].location_list[0].limit_except.excepted_methods.end());
  EXPECT_EQ(server_list[1].location_list[0].limit_except.excepted_methods.find(config::HEAD),
            server_list[1].location_list[0].limit_except.excepted_methods.end());

  const std::vector<int> expects = {4, 2, 0};
  i = 0;
  std::for_each(expects.begin(), expects.end(), [&server_list, &i](int value) {
    EXPECT_EQ(server_list[i].location_list[0].limit_except.excepted_methods.size(), value);
    ++i;
  });
}
