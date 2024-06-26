#include "ConfigHandlerTest.hpp"

// const config::Location* searchLongestMatchLocationConfig(
//                 const config::Server& server_config,
//                 const std::string& uri) const;

TEST(ConfigHandlerTestSearchLocationConfig, root) {
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http_.server_list_[0];

  test.sameLocation(server.location_list_[1],
                    test.config_handler_.searchLongestMatchLocationConfig(server, "/"));
}

TEST(ConfigHandlerTestSearchLocationConfig, samename) {
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http_.server_list_[0];

  test.sameLocation(server.location_list_[0],
                    test.config_handler_.searchLongestMatchLocationConfig(server, "/hello/"));
}

TEST(ConfigHandlerTestSearchLocationConfig, longest_match1) {
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/everyone/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http_.server_list_[0];

  test.sameLocation(server.location_list_[1],
                    test.config_handler_.searchLongestMatchLocationConfig(server, test.request_.uri_));
}

TEST(ConfigHandlerTestSearchLocationConfig, longest_match2) {
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/hello/goodnight/", {{"Host", "first_server"}}, "",
                   HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http_.server_list_[0];

  test.sameLocation(server.location_list_[0],
                    test.config_handler_.searchLongestMatchLocationConfig(server, test.request_.uri_));
}

TEST(ConfigHandlerTestSearchLocationConfig, longest_match3) {
  test::ConfigHandlerTest test("searchLocationConfig/search_location.conf");
  test.initRequest(config::GET, "/helo/goodnight/", {{"Host", "first_server"}}, "",
                   HttpRequest::PARSE_COMPLETE);
  config::Server server = test.config_handler_.config_->http_.server_list_[0];

  test.sameLocation(server.location_list_[1],
                    test.config_handler_.searchLongestMatchLocationConfig(server, test.request_.uri_));
}
