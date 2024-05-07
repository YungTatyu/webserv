#include "ConfigHandlerTest.hpp"

TEST(ConfigHandlerTestWriteErrorLog, off)
{
  test::ConfigHandlerTest test("writeErrorLog/writeErrLog_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[0]}});
  std::string file_path = "/dev/null";
  std::string msg = "aiueo\n";

  test.config_handler_.writeErrorLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  test.WRITE_NONE(file_path, msg);
}

TEST(ConfigHandlerTestWriteErrorLog, location)
{
  test::ConfigHandlerTest test("writeErrorLog/writeErrLog_test.conf",
                               "127.0.0.1",
                               8001);
  test.initRequest(config::GET, "/hello/", {{"Host", "first_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[0]}});
  std::string file_path = test.getAbsolutePath("logs/location_error.log");
  std::string msg = "kakikukeko\n";

  test.config_handler_.writeErrorLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  test.WRITE_ACCURATE(file_path, msg);
}

TEST(ConfigHandlerTestWriteErrorLog, Default)
{
  test::ConfigHandlerTest test("writeErrorLog/writeErrLog_test.conf",
                               "127.0.0.2",
                               8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[1]}});
  std::string file_path = test.getAbsolutePath("logs/error.log");
  std::string msg = "sashisuseso\n";

  test.config_handler_.writeErrorLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  test.WRITE_ACCURATE(file_path, msg);
}

TEST(ConfigHandlerTestWriteErrorLog, add_default)
{
  test::ConfigHandlerTest test("writeErrorLog/writeErrLog_test.conf",
                               "127.0.0.2",
                               8002);
  test.initRequest(config::GET, "/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[1]}});
  std::string file_path = test.getAbsolutePath("logs/error.log");
  std::string msg = "tachitsuteto\n";
  std::string expect_msg = "sashisuseso\ntachitsuteto\n";

  test.config_handler_.writeErrorLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  test.WRITE_ACCURATE(file_path, expect_msg);
}

TEST(ConfigHandlerTestWriteErrorLog, multiple_file)
{
  test::ConfigHandlerTest test("writeErrorLog/writeErrLog_test.conf",
                               "127.0.0.2",
                               8002);
  test.initRequest(config::GET, "/multiple/", {{"Host", "second_server"}}, "", HttpRequest::PARSE_COMPLETE);
  test.initTiedServer({{&test.config_handler_.config_->http.server_list[1]}});
  std::string file_path1 = test.getAbsolutePath("logs/multiple1.log");
  std::string file_path2 = test.getAbsolutePath("logs/multiple2.log");
  std::string file_path3 = test.getAbsolutePath("logs/multiple3.log");
  std::string msg = "naninuneno\n";

  test.config_handler_.writeErrorLog(test.tied_server_,
                                 test.request_.headers["Host"],
                                 test.request_.uri,
                                 msg);
  test.WRITE_ACCURATE(file_path1, msg);
  test.WRITE_ACCURATE(file_path2, msg);
  test.WRITE_ACCURATE(file_path3, msg);
}
