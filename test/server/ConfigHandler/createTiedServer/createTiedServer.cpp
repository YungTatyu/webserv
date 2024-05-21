#include "ConfigHandlerTest.hpp"

namespace test {

bool sameTiedServer(const struct TiedServer& tied1, const struct TiedServer& tied2) {
  return (tied1.servers_.size() == tied2.servers_.size() && tied1.addr_ == tied2.addr_ &&
          tied1.port_ == tied2.port_);
}
};  // namespace test

TEST(ConfigHandlerTestCreateTiedServer, three_server) {
  test::ConfigHandlerTest test("createTiedServer/createTiedServer_test.conf", "127.0.0.1", 8001);
  const config::Http* http = &test.config_handler_.config_->http;
  test.initTiedServer({&http->server_list[0], &http->server_list[1], &http->server_list[2]});

  EXPECT_TRUE(
      test::sameTiedServer(test.tied_server_, test.config_handler_.createTiedServer("127.0.0.1", 8001)));
}

TEST(ConfigHandlerTestCreateTiedServer, one_server1) {
  test::ConfigHandlerTest test("createTiedServer/createTiedServer_test.conf", "127.0.0.2", 8002);
  const config::Http* http = &test.config_handler_.config_->http;
  test.initTiedServer({
      &http->server_list[1],
  });

  EXPECT_TRUE(
      test::sameTiedServer(test.tied_server_, test.config_handler_.createTiedServer("127.0.0.2", 8002)));
}

TEST(ConfigHandlerTestCreateTiedServer, one_server2) {
  test::ConfigHandlerTest test("createTiedServer/createTiedServer_test.conf", "127.0.0.3", 8003);
  const config::Http* http = &test.config_handler_.config_->http;
  test.initTiedServer({
      &http->server_list[4],
  });

  EXPECT_TRUE(
      test::sameTiedServer(test.tied_server_, test.config_handler_.createTiedServer("127.0.0.3", 8003)));
}
