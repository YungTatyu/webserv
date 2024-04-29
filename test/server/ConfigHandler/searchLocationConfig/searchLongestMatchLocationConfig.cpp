#include "ConfigHandlerTest.hpp"

//const config::Location* searchLongestMatchLocationConfig(
//                const config::Server& server_config,
//                const std::string& uri) const;

TEST(ConfigHandlerTest, searchLongestMatchLocationConfig_root)
{
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http.server_list[0];

  EXPECT_TRUE(test.sameLocation(
          server.location_list[0],
          test.config_handler_.searchLongestMatchLocationConfig(server, "/")));
}

TEST(ConfigHandlerTest, searchLongestMatchLocationConfig_samename)
{
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http.server_list[0];

  EXPECT_TRUE(test.sameLocation(
          server.location_list[1],
          test.config_handler_.searchLongestMatchLocationConfig(server, "/hello/")));
}

TEST(ConfigHandlerTest, searchLongestMatchLocationConfig_longest_match1)
{
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http.server_list[0];

  EXPECT_TRUE(test.sameLocation(
          server.location_list[0],
          test.config_handler_.searchLongestMatchLocationConfig(server, "/everyone/")));
}

TEST(ConfigHandlerTest, searchLongestMatchLocationConfig_longest_match2)
{
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http.server_list[0];

  EXPECT_TRUE(test.sameLocation(
          server.location_list[1],
          test.config_handler_.searchLongestMatchLocationConfig(server, "/hello/goodnight/")));
}
