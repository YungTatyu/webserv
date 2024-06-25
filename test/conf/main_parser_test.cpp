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
  const config::Http &http = config->http_;
  const config::Events &events = config->events_;
  const std::vector<config::Server> &server_list = http.server_list_;
  int i;

  // http
  EXPECT_NE(config->directives_set_.find(HTTP), config->directives_set_.end());

  // events
  EXPECT_NE(config->directives_set_.find(EVENTS), config->directives_set_.end());

  // server
  EXPECT_NE(http.directives_set_.find(SERVER), http.directives_set_.end());

  std::for_each(server_list.begin(), server_list.end() - 1, [](config::Server server) {
    EXPECT_NE(server.directives_set_.find(LOCATION), server.directives_set_.end());
  });

  EXPECT_EQ(server_list[3].directives_set_.find(LOCATION), server_list[3].directives_set_.end());
  EXPECT_EQ(server_list.size(), 4);

  // location
  EXPECT_EQ(server_list[0].location_list_[0].uri_, "path");
  EXPECT_EQ(server_list[1].location_list_[0].uri_, "");

  std::vector<std::string> expect_str = {
      "path1", "path2", "path3", "path4", "path5", "path6", "path7",
  };
  i = 0;
  std::for_each(expect_str.begin(), expect_str.end(), [&server_list, &i](std::string expect) {
    EXPECT_EQ(server_list[2].location_list_[i].uri_, expect);
    ++i;
  });

  std::vector<int> expect_size = {1, 1, 7, 0};
  i = 0;
  std::for_each(expect_size.begin(), expect_size.end(), [&server_list, &i](int expect) {
    EXPECT_EQ(server_list[i].location_list_.size(), expect);
    ++i;
  });

  // limit except
  EXPECT_NE(server_list[0].location_list_[0].directives_set_.find(LIMIT_EXCEPT),
            server_list[0].location_list_[0].directives_set_.end());
  EXPECT_NE(server_list[1].location_list_[0].directives_set_.find(LIMIT_EXCEPT),
            server_list[1].location_list_[0].directives_set_.end());
  for (size_t i = 0; i < server_list[2].location_list_.size(); i++) {
    EXPECT_EQ(server_list[2].location_list_[i].directives_set_.find(LIMIT_EXCEPT),
              server_list[2].location_list_[i].directives_set_.end());
  }

  std::vector<config::REQUEST_METHOD> expect_methods = {
      config::GET,
      config::POST,
      config::DELETE,
      config::HEAD,
  };
  std::for_each(expect_methods.begin(), expect_methods.end(), [&server_list](config::REQUEST_METHOD expect) {
    EXPECT_NE(server_list[0].location_list_[0].limit_except_.excepted_methods_.find(expect),
              server_list[0].location_list_[0].limit_except_.excepted_methods_.end());
  });

  EXPECT_EQ(server_list[1].location_list_[0].limit_except_.excepted_methods_.find(config::GET),
            server_list[1].location_list_[0].limit_except_.excepted_methods_.end());
  EXPECT_NE(server_list[1].location_list_[0].limit_except_.excepted_methods_.find(config::POST),
            server_list[1].location_list_[0].limit_except_.excepted_methods_.end());
  EXPECT_NE(server_list[1].location_list_[0].limit_except_.excepted_methods_.find(config::DELETE),
            server_list[1].location_list_[0].limit_except_.excepted_methods_.end());
  EXPECT_EQ(server_list[1].location_list_[0].limit_except_.excepted_methods_.find(config::HEAD),
            server_list[1].location_list_[0].limit_except_.excepted_methods_.end());

  const std::vector<int> expects = {4, 2, 0};
  i = 0;
  std::for_each(expects.begin(), expects.end(), [&server_list, &i](int value) {
    EXPECT_EQ(server_list[i].location_list_[0].limit_except_.excepted_methods_.size(), value);
    ++i;
  });
}
