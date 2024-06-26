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
#include "conf.hpp"
#include "utils.hpp"

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
    delete config_handler_.config_;

    // socket 処理
    close(sockfd[0]);
    close(sockfd[1]);
  }

  void initRequest(const test::string_map_case_insensitive &headers, const config::REQUEST_METHOD &method,
                   const std::string &uri, const HttpRequest::ParseState state, const std::string &body = "",
                   const std::string &queries = "", const std::string &version = "HTTP/1.1") {
    this->request_.headers_ = headers;
    this->request_.uri_ = uri;
    this->request_.parse_state_ = state;
    this->request_.body_ = body;
    this->request_.queries_ = queries;
    this->request_.version_ = version;
    this->request_.method_ = method;
  }

  std::string createHeader(const std::string &status_code_line) const {
    const string_map_case_insensitive headers = this->response_.headers_;

    std::string res = "HTTP/1.1 ";
    res += (status_code_line + "\r\n");
    for (string_map_case_insensitive::const_iterator it = headers.begin(); it != headers.end(); ++it)
      res += (toTitleCase(it->first) + ": " + it->second + "\r\n");
    res += "\r\n";
    return res;
  }

  std::string toTitleCase(const std::string &input) const {
    std::string re = input;
    std::string::iterator it = std::find(re.begin(), re.end(), '-');
    while (it != re.end()) {
      ++it;
      if (it == re.end()) break;
      *it = std::toupper(*it);
      it = std::find(it, re.end(), '-');
    }
    return re;
  }

  void ASSERT_CORRECT_RESPONSE(const std::string &expect, const std::string &actual) {
    ASSERT_EQ(expect, actual);
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
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[200]);
  expect_res += utils::readFile("test/server/HttpResponse/mix/file/aliasHtml/alias.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_and_root_in_loc) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/location-root/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string expect_res;
  std::string final_response;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[200]);
  expect_res += utils::readFile("test/server/HttpResponse/mix/file/index.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_internal_redirect) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/redirect/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[200]);
  expect_res += utils::readFile("test/server/HttpResponse/mix/file/internal_redirect.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_error_page) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/code-error-page/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[405]);
  expect_res += utils::readFile("test/server/HttpResponse/mix/file/index.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

/*
 * HEAD method ver.
 */

TEST_F(HttpResponseMix, try_files_and_alias_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/alias/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[200]);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_and_root_in_loc_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/location-root/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[200]);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_internal_redirect_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/redirect/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[200]);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseMix, try_files_error_page_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/code-error-page/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[405]);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}
};  // namespace test
