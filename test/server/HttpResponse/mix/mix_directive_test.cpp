#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "ConfigHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ResponseTest.hpp"
#include "Utils.hpp"
#include "conf.hpp"

namespace test {

class HttpResponseMix : public ::testing::Test {
 protected:
  // 各TESTの前に呼び出されるセットアップメソッド
  void SetUp() override {
    std::string file_path;
    file_path = "test/server/HttpResponse/mix/file/mix_directive.conf";

    const config::Main *config = config::initConfig(file_path);
    config_handler_.loadConfiguration(config);

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockfd) == -1) {
      perror("socketpair");
      delete config;
      GTEST_SKIP();
    }
  }

  void TearDown() override {
    // テストケースのクリーンアップ処理
    const testing::TestInfo *test_info = testing::UnitTest::GetInstance()->current_test_info();

    // config handler　削除
    delete config_handler_.config_;

    // socket 処理
    close(sockfd[0]);
    close(sockfd[1]);
  }

  void initRequest(const test::string_map_case_insensitive &headers, const config::REQUEST_METHOD &method,
                   const std::string &uri, const HttpRequest::ParseState state, const std::string &body = "",
                   const std::string &queries = "", const std::string &version = "HTTP/1.1") {
    this->request_.headers = headers;
    this->request_.uri = uri;
    this->request_.parseState = state;
    this->request_.body = body;
    this->request_.queries = queries;
    this->request_.version = version;
    this->request_.method = method;
  }

  void ASSERT_CORRECT_RESPONSE(const std::vector<std::string> &expect, const std::string &actual) {
    for (size_t i = 0; i < expect.size(); i++) {
      ASSERT_TRUE(actual.find(expect[i]) != std::string::npos);
    }
  }

  ConfigHandler config_handler_;
  HttpRequest request_;
  HttpResponse response_;
  TiedServer tied_server_;
  int sockfd[2];
};

TEST_F(HttpResponseMix, try_files_and_alias) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/alias/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::vector<std::string> expect_res;
  std::string final_response;

  expect_res.push_back("HTTP/1.1 200 OK");
  expect_res.push_back("Server: webserv/1.0");
  expect_res.push_back("Connection: keep-alive");
  expect_res.push_back("Content-Type: text/html");
  expect_res.push_back("Content-Length: 286");
  std::ifstream ifs("test/server/HttpResponse/mix/file/aliasHtml/alias.html");
  ASSERT_TRUE(ifs.is_open());
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  ifs.close();
  expect_res.push_back(buffer.str());

  // 関数適用
  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_and_root_in_loc) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/location-root/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::vector<std::string> expect_res;
  std::string final_response;

  expect_res.push_back("HTTP/1.1 200 OK");
  expect_res.push_back("Server: webserv/1.0");
  expect_res.push_back("Connection: keep-alive");
  expect_res.push_back("Content-Type: text/html");
  expect_res.push_back("Content-Length: 272");
  std::ifstream ifs("test/server/HttpResponse/mix/file/index.html");
  ASSERT_TRUE(ifs.is_open());
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  ifs.close();
  expect_res.push_back(buffer.str());

  // 関数適用
  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_internal_redirect) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/redirect/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::vector<std::string> expect_res;
  std::string final_response;

  expect_res.push_back("HTTP/1.1 200 OK");
  expect_res.push_back("Server: webserv/1.0");
  expect_res.push_back("Connection: keep-alive");
  expect_res.push_back("Content-Type: text/html");
  expect_res.push_back("Content-Length: 274");
  std::ifstream ifs("test/server/HttpResponse/mix/file/internal_redirect.html");
  ASSERT_TRUE(ifs.is_open());
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  ifs.close();
  expect_res.push_back(buffer.str());

  // 関数適用
  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_error_page) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/code-error-page/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::vector<std::string> expect_res;
  std::string final_response;

  expect_res.push_back("HTTP/1.1 405 Not Allowed");
  expect_res.push_back("Server: webserv/1.0");
  expect_res.push_back("Connection: close");
  expect_res.push_back("Content-Type: text/html");
  expect_res.push_back("Content-Length: 272");
  std::ifstream ifs("test/server/HttpResponse/mix/file/index.html");
  ASSERT_TRUE(ifs.is_open());
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  ifs.close();
  expect_res.push_back(buffer.str());

  // 関数適用
  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}
};  // namespace test
