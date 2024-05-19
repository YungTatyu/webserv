#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTestSearchRootPath, root) {
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/root/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[0];

  // POST method 以外のメソッドを192.168.0.1以外制限
  // EXPECT_EQ(test.getAbsolutePath(".") + "alias/", test.searchRootPath(server, location));
  EXPECT_EQ("/var/www/root", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTestSearchRootPath, alias) {
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/image/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[1];

  // POST method 以外のメソッドを192.168.0.1以外制限
  // EXPECT_EQ(test.getAbsolutePath(".") + "alias/", test.searchRootPath(server, location));
  EXPECT_EQ("/var/www/data/", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTestSearchRootPath, server) {
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/no-root/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[2];

  EXPECT_EQ("/var/www/html/server1", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTestSearchRootPath, http) {
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/", {{"Host", "second-server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[1];
  const config::Location* location = nullptr;

  // POST method 以外のメソッドを192.168.0.1以外制限
  // EXPECT_EQ(test.getAbsolutePath(".") + "alias/", test.searchRootPath(server, location));
  EXPECT_EQ("/var/www/html", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTestSearchRootPath, relative_path) {
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/relative-root/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[3];

  EXPECT_EQ("test/server/HttpResponse/RootPath/file", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTestSearchRootPath, no_root_directive) {
  test::ConfigHandlerTest test("RootPath/test2.conf");
  test.initRequest(config::GET, "/no-directive/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[0];

  EXPECT_EQ("html", test.config_handler_.searchRootPath(server, location));
}
