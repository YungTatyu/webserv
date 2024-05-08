#include "ResponseTest.hpp"

TEST(HttpResponseTestWriteAccessLog, log_format1)
{
  test::ResponseTest test("test/server/HttpResponse/AccessLog/file/test.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "_"}, {"User-Agent", "Mozilla/5.0"}},
      {config::REQUEST_METHOD::GET}, "/", HttpRequest::PARSE_COMPLETE));

  std::string file_path = test.getAbsolutePath("logs/format.log");

  // IPv4アドレスの文字列表現をバイナリ形式に変換
  struct in_addr addr;
  std::string ipAddress = "127.0.0.1";
  inet_pton(AF_INET, ipAddress.c_str(), &addr);
  test.config_handler_.writeAccessLog(
      test.config_handler_.config_->http.server_list[0],
      &test.config_handler_.config_->http.server_list[0].location_list[0],
      test.config_handler_.createAcsLogMsg(addr.s_addr, 200, test.responses_[0].body_.size(), test.request_)
      );
  std::string body = Utils::readFile("test/server/HttpResponse/AccessLog/file/index.html");
  test.testAccessLogEntry(
    file_path,
    "127.0.0.1",
    "GET",
    "/",
    1.1,
    200,
    body.size(),
    "Mozilla/5.0");
}

TEST(HttpResponseTestWriteAccessLog, log_format2)
{
  test::ResponseTest test("test/server/HttpResponse/AccessLog/file/test.conf");
  ASSERT_NO_FATAL_FAILURE(test.setUpAll(
      {{"127.0.0.1", 4242}, {"127.0.0.1", 4243}}, {{"host", "_"}, {"User-Agent", "curl/7.68.0"}},
      {config::REQUEST_METHOD::GET}, "/error/", HttpRequest::PARSE_COMPLETE));

  std::string file_path = test.getAbsolutePath("logs/format.log");

  // IPv4アドレスの文字列表現をバイナリ形式に変換
  struct in_addr addr;
  std::string ipAddress = "127.0.0.1";
  inet_pton(AF_INET, ipAddress.c_str(), &addr);
  test.config_handler_.writeAccessLog(
      test.config_handler_.config_->http.server_list[0],
      &test.config_handler_.config_->http.server_list[0].location_list[1],
      test.config_handler_.createAcsLogMsg(addr.s_addr, 404, test.responses_[0].body_.size(), test.request_)
      );
  std::string body = test.createDefaultErrorBody(404);
  test.testAccessLogEntry(
    file_path,
    "127.0.0.1",
    "GET",
    "/error/",
    1.1,
    404,
    body.size(),
    "curl/7.68.0");
}
