#include "ConfigHandlerTest.hpp"

namespace test {

typedef std::vector<config::REQUEST_METHOD>::iterator method_it;
typedef std::vector<std::string>::iterator ip_it;
typedef std::vector<std::vector<int>>::iterator vec_expect_it;
typedef std::vector<int>::iterator expect_it;

void assert_allowRequest_loop(ConfigHandlerTest& test, int server_key, int loc_key,
                              std::vector<config::REQUEST_METHOD> vec_method, std::vector<std::string> vec_ip,
                              std::vector<std::vector<int>> vec_vec_expect) {
  const config::Server& server = test.config_handler_.config_->http.server_list[server_key];
  const config::Location* location =
      &test.config_handler_.config_->http.server_list[server_key].location_list[loc_key];

  method_it method = vec_method.begin();
  vec_expect_it vec_expect = vec_vec_expect.begin();
  for (; method != vec_method.end() && vec_expect != vec_vec_expect.end(); method++, vec_expect++) {
    test.request_.method = *method;
    ip_it ip = vec_ip.begin();
    expect_it expect = (*vec_expect).begin();
    for (; ip != vec_ip.end() && expect != (*vec_expect).end(); ip++, expect++) {
      EXPECT_EQ(*expect,
                test.config_handler_.allowRequest(server, location, test.request_, test.createClient(*ip)));
    }
  }
}
};  // namespace test

TEST(ConfigHandlerTestAllowRequest, limit_except_POST) {
  test::ConfigHandlerTest test("allowRequest/limit_method.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);

  // POST method 以外のメソッドを192.168.0.1以外制限
  test::assert_allowRequest_loop(test, 0, 0, {config::GET, config::POST, config::DELETE, config::HEAD},
                                 {"192.168.0.1", "192.168.0.2"},
                                 {{ConfigHandler::ACCESS_ALLOW, ConfigHandler::METHOD_DENY},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::METHOD_DENY},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::METHOD_DENY}});
}

TEST(ConfigHandlerTestAllowRequest, limit_except_DELETE) {
  test::ConfigHandlerTest test("allowRequest/limit_method.conf");
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);

  // DELETE method 以外のメソッドを192.168.0.1に対して制限
  test::assert_allowRequest_loop(test, 0, 1, {config::GET, config::POST, config::DELETE, config::HEAD},
                                 {"192.168.0.1", "192.168.0.2"},
                                 {{ConfigHandler::METHOD_DENY, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::METHOD_DENY, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::METHOD_DENY, ConfigHandler::ACCESS_ALLOW}});
}

TEST(ConfigHandlerTestAllowRequest, limit_except_do_nothing) {
  test::ConfigHandlerTest test("allowRequest/limit_method.conf");
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);

  // DELETE method 以外のメソッドを192.168.0.1に対して制限
  test::assert_allowRequest_loop(test, 1, 0, {config::GET, config::POST, config::DELETE, config::HEAD},
                                 {"192.168.0.1", "192.168.0.2"},
                                 {{ConfigHandler::ACCESS_ALLOW, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::ACCESS_ALLOW},
                                  {ConfigHandler::ACCESS_ALLOW, ConfigHandler::ACCESS_ALLOW}});
}
