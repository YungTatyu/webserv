#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTestAllowRequest, allow_all) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_TRUE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[0],
                  &test.config_handler_.config_->http.server_list[0].location_list[0],
                  test.request_,
                  test.createClient("192.168.0.1"))
             );
}

TEST(ConfigHandlerTestAllowRequest, deny_all) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_FALSE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[0],
                  &test.config_handler_.config_->http.server_list[0].location_list[1],
                  test.request_,
                  test.createClient("192.168.0.1"))
             );
}

TEST(ConfigHandlerTestAllowRequest, allow_one_address1) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_TRUE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[1],
                  &test.config_handler_.config_->http.server_list[1].location_list[0],
                  test.request_,
                  test.createClient("192.168.0.2"))
             );
}

TEST(ConfigHandlerTestAllowRequest, deny_one_address1) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_FALSE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[1],
                  &test.config_handler_.config_->http.server_list[1].location_list[1],
                  test.request_,
                  test.createClient("192.168.0.2"))
             );
}

TEST(ConfigHandlerTestAllowRequest, deny_one_address2) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/", {{"Host", "third_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_FALSE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[2],
                  &test.config_handler_.config_->http.server_list[2].location_list[0],
                  test.request_,
                  test.createClient("192.168.0.3"))
             );
}

TEST(ConfigHandlerTestAllowRequest, allow_one_address2) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "third_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_TRUE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[2],
                  &test.config_handler_.config_->http.server_list[2].location_list[1],
                  test.request_,
                  test.createClient("192.168.0.4"))
             );
}

