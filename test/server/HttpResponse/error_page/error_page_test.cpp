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

class HttpResponseErrorPage : public ::testing::Test {
 protected:
  // 各TESTの前に呼び出されるセットアップメソッド
  void SetUp() override {
    std::string file_path;
    const testing::TestInfo *test_info = testing::UnitTest::GetInstance()->current_test_info();
    if (static_cast<std::string>(test_info->name()).find("dup_error_num") != std::string::npos) {
      file_path = "test/server/HttpResponse/error_page/file/ErrorPage2.conf";
    } else {
      file_path = "test/server/HttpResponse/error_page/file/ErrorPage.conf";
    }

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

    // config handler　削除
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

  std::string createDefaultErrorBody(unsigned int status_code) const {
    const std::string webserv_error_page_tail = "<hr><center>webserv/1.0</center>\r\n</body>\r\n</html>\r\n";
    return *(HttpResponse::default_error_page_map_[status_code]) + webserv_error_page_tail;
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

TEST_F(HttpResponseErrorPage, code_response_uri1) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/nothing.html", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader("499");
  expect_res += utils::readFile("test/server/HttpResponse/error_page/file/40x.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, code_response_uri2) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/method-error/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader("499");
  expect_res += utils::readFile("test/server/HttpResponse/error_page/file/40x.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, code_uri) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/bad-reqeust/", HttpRequest::PARSE_ERROR);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[400]);
  expect_res += utils::readFile("test/server/HttpResponse/error_page/file/internal_redirect.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, eternal_redirect) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/permanently_internal_redirect/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[500]);
  expect_res += createDefaultErrorBody(500);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, dup_error_num) {
  // ErrorPage2.conf
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::GET,
              "/nothing/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4243);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader("499");
  expect_res += utils::readFile("test/server/HttpResponse/error_page/file/40x.html");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

/*
 * HEAD method ver.
 */

TEST_F(HttpResponseErrorPage, code_response_uri1_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/nothing.html", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader("499");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, code_response_uri2_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/method-error/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader("499");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, code_uri_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/bad-reqeust/", HttpRequest::PARSE_ERROR);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[400]);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, eternal_redirect_HEAD) {
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/permanently_internal_redirect/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4242);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader(HttpResponse::status_line_map_[500]);

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}

TEST_F(HttpResponseErrorPage, dup_error_num_HEAD) {
  // ErrorPage2.conf
  initRequest({{"host", "test_server"}, {"User-Agent", "Mozilla/5.0"}}, config::REQUEST_METHOD::HEAD,
              "/nothing/", HttpRequest::PARSE_COMPLETE);
  tied_server_ = config_handler_.createTiedServer("127.0.0.1", 4243);

  // 正解response
  std::string final_response;
  std::string expect_res;

  final_response =
      HttpResponse::generateResponse(request_, response_, tied_server_, sockfd[0], config_handler_);
  expect_res = createHeader("499");

  // 結果確認
  ASSERT_CORRECT_RESPONSE(expect_res, final_response);
}
};  // namespace test
