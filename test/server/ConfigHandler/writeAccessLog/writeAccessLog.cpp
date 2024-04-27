#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTest, writeAccessLog_off)
{
  test::ConfigHandlerTest test("writeAccessLog/writeAcsLog_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[0]}});
  std::string file_path = test.getAbsolutePath(".") + "/off";
  std::string msg = "aiueo";

  test.config_handler_.writeAccessLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  EXPECT_FALSE(test.WRITE_ACCURATE(file_path, msg));
}

TEST(ConfigHandlerTest, writeAccessLog_location)
{
  test::ConfigHandlerTest test("writeAccessLog/writeAcsLog_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[0]}});
  std::string file_path = test.getAbsolutePath("./logs/location_access.log");
  std::string msg = "kakikukeko";

  test.config_handler_.writeAccessLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  EXPECT_TRUE(test.WRITE_ACCURATE(file_path, msg));
}

TEST(ConfigHandlerTest, writeAccessLog_parent_context)
{
  test::ConfigHandlerTest test("writeAccessLog/writeAcsLog_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/goodnight/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[0]}});
  std::string file_path = test.getAbsolutePath("./logs/server_access.log");
  std::string msg = "sashisuseso";

  test.config_handler_.writeAccessLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  EXPECT_TRUE(test.WRITE_ACCURATE(file_path, msg));
}

TEST(ConfigHandlerTest, writeAccessLog_grand_parent_context)
{
  test::ConfigHandlerTest test("writeAccessLog/writeAcsLog_test.conf",
                               "127.0.0.2",
                               8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[1]}});
  std::string file_path = test.getAbsolutePath("./logs/http_access.log");
  std::string msg = "tachitsuteto";

  test.config_handler_.writeAccessLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  EXPECT_TRUE(test.WRITE_ACCURATE(file_path, msg));
}
