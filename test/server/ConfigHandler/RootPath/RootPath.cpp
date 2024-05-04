#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTest, searchRootPath_root)
{
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/root/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[0];

  // POST method 以外のメソッドを192.168.0.1以外制限
  //EXPECT_EQ(test.getAbsolutePath(".") + "alias/", test.searchRootPath(server, location));
  EXPECT_EQ("/var/www/root", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTest, searchRootPath_alias)
{
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/image/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[1];

  // POST method 以外のメソッドを192.168.0.1以外制限
  //EXPECT_EQ(test.getAbsolutePath(".") + "alias/", test.searchRootPath(server, location));
  EXPECT_EQ("/var/www/data/", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTest, searchRootPath_server)
{
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/no-root/", {{"Host", ""}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[0];
  const config::Location* location = &test.config_handler_.config_->http.server_list[0].location_list[2];

  EXPECT_EQ("/var/www/html/server1", test.config_handler_.searchRootPath(server, location));
}

TEST(ConfigHandlerTest, searchRootPath_http)
{
  test::ConfigHandlerTest test("RootPath/test1.conf");
  test.initRequest(config::GET, "/", {{"Host", "second-server"}}, "", HttpRequest::PARSE_COMPLETE);
  const config::Server& server = test.config_handler_.config_->http.server_list[1];
  const config::Location* location = nullptr;

  // POST method 以外のメソッドを192.168.0.1以外制限
  //EXPECT_EQ(test.getAbsolutePath(".") + "alias/", test.searchRootPath(server, location));
  EXPECT_EQ("/var/www/html", test.config_handler_.searchRootPath(server, location));
}
