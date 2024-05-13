#include "ConfigHandlerTest.hpp"
#include "Listen.hpp"
#include "ServerName.hpp"

/* const config::Server& searchServerConfig(const struct TiedServer& tied_servers,
 *                                          const std::string& server_name) const;
 */

// カスタムアサーション
#define ASSERT_ANY(...)                                             \
  {                                                                 \
    bool anyCondition = false;                                      \
    bool conditions[] = {__VA_ARGS__};                              \
    int numConditions = sizeof(conditions) / sizeof(conditions[0]); \
    for (int i = 0; i < numConditions; ++i) {                       \
      if (conditions[i]) {                                          \
        anyCondition = true;                                        \
        break;                                                      \
      }                                                             \
    }                                                               \
    ASSERT_TRUE(anyCondition);                                      \
  }

namespace test {

void sameServer(const config::Server& expect, const config::Server& actual) {
  // sizeが等しいか
  ASSERT_EQ(expect.directives_set, actual.directives_set);
  ASSERT_EQ(expect.listen_list.size(), actual.listen_list.size());
  ASSERT_EQ(expect.server_name.getName().size(), actual.server_name.getName().size());

  // Listen address/port　と server_nameが等しいか
  std::vector<config::Listen>::const_iterator expect_lit = expect.listen_list.begin();
  std::set<std::string>::const_iterator expect_nit = expect.server_name.getName().begin();
  std::vector<config::Listen>::const_iterator actual_lit = actual.listen_list.begin();
  std::set<std::string>::const_iterator actual_nit = actual.server_name.getName().begin();
  for (; expect_lit != expect.listen_list.end(), actual_lit != actual.listen_list.end();
       expect_lit++, actual_lit++) {
    ASSERT_EQ(expect_lit->getAddress(), actual_lit->getAddress());
    ASSERT_EQ(expect_lit->getport(), actual_lit->getport());
  }

  for (; expect_nit != expect.server_name.getName().end(), actual_nit != actual.server_name.getName().end();
       expect_nit++, actual_nit++) {
    ASSERT_EQ(*expect_nit, *actual_nit);
  }
}

};  // namespace test

TEST(ConfigHandlerTestSearchServerConfig, same_port) {
  test::ConfigHandlerTest test("searchServerConfig/test.conf");
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Http& http = test.config_handler_.config_->http;
  test.initTiedServer({&http.server_list[0]});

  test::sameServer(http.server_list[0],
                   test.config_handler_.searchServerConfig(test.tied_server_, test.request_.headers["Host"]));
}

TEST(ConfigHandlerTestSearchServerConfig, same_address) {
  test::ConfigHandlerTest test("searchServerConfig/test.conf");
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Http& http = test.config_handler_.config_->http;
  test.initTiedServer({&http.server_list[1]});

  test::sameServer(http.server_list[1],
                   test.config_handler_.searchServerConfig(test.tied_server_, test.request_.headers["Host"]));
}

TEST(ConfigHandlerTestSearchServerConfig, same_server_name) {
  test::ConfigHandlerTest test("searchServerConfig/test.conf");
  test.initRequest(config::GET, "/", {{"Host", "third_server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Http& http = test.config_handler_.config_->http;
  test.initTiedServer({&http.server_list[2], &http.server_list[3]});

  test::sameServer(http.server_list[2],
                   test.config_handler_.searchServerConfig(test.tied_server_, test.request_.headers["Host"]));
}

TEST(ConfigHandlerTestSearchServerConfig, default_server) {
  test::ConfigHandlerTest test("searchServerConfig/test2.conf");
  test.initRequest(config::GET, "/", {{"Host", "a"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Http& http = test.config_handler_.config_->http;
  test.initTiedServer({&http.server_list[0], &http.server_list[1]});

  test::sameServer(http.server_list[1],
                   test.config_handler_.searchServerConfig(test.tied_server_, test.request_.headers["Host"]));
}

TEST(ConfigHandlerTestSearchServerConfig, default_server_same_server_name) {
  test::ConfigHandlerTest test("searchServerConfig/test2.conf");
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Http& http = test.config_handler_.config_->http;
  test.initTiedServer({&http.server_list[0], &http.server_list[1], &http.server_list[2]});

  test::sameServer(http.server_list[0],
                   test.config_handler_.searchServerConfig(test.tied_server_, test.request_.headers["Host"]));
}

TEST(ConfigHandlerTestSearchServerConfig, duplicate_server) {
  test::ConfigHandlerTest test("searchServerConfig/test2.conf");
  test.initRequest(config::GET, "/", {{"Host", "third_server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Http& http = test.config_handler_.config_->http;
  test.initTiedServer({&http.server_list[0], &http.server_list[1], &http.server_list[2]});

  test::sameServer(http.server_list[1],
                   test.config_handler_.searchServerConfig(test.tied_server_, test.request_.headers["Host"]));
}
