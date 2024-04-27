#include "ConfigHandlerTest.hpp"

namespace test {

struct sockaddr_in createClient(std::string ip)
{
  struct sockaddr_in cli_addr;
  std::memset(&cli_addr, 0, sizeof(cli_addr));  // ゼロで初期化
  cli_addr.sin_family = AF_INET;                 // IPv4
  cli_addr.sin_addr.s_addr = Utils::StrToIPAddress(ip);
  return cli_addr;
}

}; // namespace test

TEST(ConfigHandlerTest, allowRequest_allow_all) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_TRUE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[0],
                  &test.config_handler_.config_->http.server_list[0].location_list[0],
                  test.request_,
                  test::createClient("192.168.0.1"))
             );
}

TEST(ConfigHandlerTest, allowRequest_deny_all) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_FALSE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[0],
                  &test.config_handler_.config_->http.server_list[0].location_list[1],
                  test.request_,
                  test::createClient("192.168.0.1"))
             );
}

TEST(ConfigHandlerTest, allowRequest_allow_one_address1) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_TRUE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[1],
                  &test.config_handler_.config_->http.server_list[1].location_list[0],
                  test.request_,
                  test::createClient("192.168.0.2"))
             );
}

TEST(ConfigHandlerTest, allowRequest_deny_one_address1) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_FALSE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[1],
                  &test.config_handler_.config_->http.server_list[1].location_list[1],
                  test.request_,
                  test::createClient("192.168.0.2"))
             );
}

TEST(ConfigHandlerTest, allowRequest_deny_one_address2) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/", {{"Host", "third_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_FALSE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[2],
                  &test.config_handler_.config_->http.server_list[2].location_list[0],
                  test.request_,
                  test::createClient("192.168.0.3"))
             );
}

TEST(ConfigHandlerTest, allowRequest_allow_one_address2) {
  test::ConfigHandlerTest test("allowRequest/allowRequest_test.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "third_server"}}, "", HttpRequest::PARSE_COMPLETE);

  EXPECT_TRUE(test.config_handler_.allowRequest(
                  test.config_handler_.config_->http.server_list[2],
                  &test.config_handler_.config_->http.server_list[2].location_list[1],
                  test.request_,
                  test::createClient("192.168.0.4"))
             );
}
