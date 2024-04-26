#ifndef TEST_HTTP_RESPONSE_TEST_HPP
#define TEST_HTTP_RESPONSE_TEST_HPP

#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "ConfigHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

namespace test {

typedef std::pair<std::string, unsigned int> ip_address_pair;
typedef std::map<std::string, std::string, Utils::CaseInsensitiveCompare> string_map_case_insensitive;
typedef std::map<std::string, std::string> string_map;

class ResponseTest {
 private:
  ResponseTest();
  void err(const std::string &err_msg) const { FAIL() << err_msg; }

 public:
  ResponseTest(const std::string &conf_path) : conf_path_(conf_path) {}

  ~ResponseTest() {
    delete this->config_handler_.config_;
    close(this->sockets_[0]);
    close(this->sockets_[1]);
  }

  /**
   * @brief Set Up object
   *
   * constructorでASSERT_NE()やFAIL()を呼べないのでここで処理する
   *
   */
  void setUp() {
    this->config_handler_.loadConfiguration(config::initConfig(this->conf_path_));
    ASSERT_NE(this->config_handler_.config_, nullptr);
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockets_) == -1)
      err(std::string("socketpair(): ") + std::strerror(errno));
  }

  /**
   * @brief Set the Up All objects and create response
   * 
   * @param ip_addresses 
   * @param headers 
   * @param methods
   * @param uri 
   * @param state 
   * @param body 
   * @param queries 
   * @param version 
   */
  void setUpAll(const std::vector<ip_address_pair> &ip_addresses, const string_map_case_insensitive &headers, const std::vector<config::REQUEST_METHOD> &methods,
                const std::string &uri, const HttpRequest::ParseState state, const std::string &body = "",
                const std::string &queries = "", const std::string &version = "HTTP/1.1") {
    ASSERT_NO_FATAL_FAILURE(setUp());
    initTiedServers(ip_addresses);
    initRequest(headers, methods, uri, state, body, queries, version);
    generateResponse();
  }

  /**
   * @brief Tied Serversを初期化する
   *
   * @param ip_addresses テストしたいIP Adress
   */
  void initTiedServers(const std::vector<ip_address_pair> &ip_addresses) {
    for (std::vector<ip_address_pair>::const_iterator it = ip_addresses.begin(); it != ip_addresses.end();
         ++it)
      this->tied_servers_.push_back(this->config_handler_.createTiedServer(it->first, it->second));
    this->responses_.resize(this->responses_.size() + ip_addresses.size());
  }

  /**
   * @brief HttpRequestのメンバ変数を初期化する
   * 
   * @param headers 
   * @param methods 
   * @param uri 
   * @param state 
   * @param body 
   * @param queries 
   * @param version 
   */
  void initRequest(const string_map_case_insensitive &headers, const std::vector<config::REQUEST_METHOD> &methods, const std::string &uri,
                   const HttpRequest::ParseState state, const std::string &body = "",
                   const std::string &queries = "", const std::string &version = "HTTP/1.1") {
    this->request_.headers = headers;
    this->request_.uri = uri;
    this->request_.parseState = state;
    this->request_.body = body;
    this->request_.queries = queries;
    this->request_.version = version;
    std::for_each(methods.begin(), methods.end(), [this](config::REQUEST_METHOD method) {
      this->methods_.push_back(method);
    });
    if (methods.size() <= 1)
      return;
    this->responses_.resize(this->responses_.size() + methods.size());
  }

  /**
   * @brief testを実行する前に呼ぶ必要がある
   *
   */
  void generateResponse() {
    int i = 0;
    std::for_each(this->tied_servers_.begin(), this->tied_servers_.end(), [this, &i](TiedServer tied_server) { // testするip adressの数だけloop
      std::for_each(this->methods_.begin(), this->methods_.end(), [this, &i, &tied_server](config::REQUEST_METHOD method) { // testするmethodの数だけloop
        this->request_.method = method; // testするmethodを変える
        this->final_responses_.push_back(HttpResponse::generateResponse(
            this->request_, this->responses_[i], tied_server, this->sockets_[0], this->config_handler_));
        ++i;
      });
    });
  }

  /**
   * @brief headerをtestする
   *
   * Dateに関しては、現在のGMTと比較する
   * そのため、引数に渡されるDateのvalueは無視される
   *
   * @param expects
   */
  void testHeaders(const string_map &expects) const {
    std::for_each(this->responses_.begin(), this->responses_.end(), [&expects, this](HttpResponse response) {
      const char *date = "Date";
      for (string_map::const_iterator it = expects.begin(); it != expects.end(); ++it) {
        EXPECT_NO_THROW(if (Utils::compareIgnoreCase(it->first, date)) {
          this->testDate(response.headers_[date]);
          continue;
        } EXPECT_EQ(response.headers_.at(it->first), it->second));
      }
      EXPECT_EQ(response.headers_.size(), expects.size());
    });
  }

  void testBody(const std::string &expect) const {
    std::for_each(this->responses_.begin(), this->responses_.end(),
                  [&expect](HttpResponse response) { EXPECT_EQ(response.body_, expect); });
  }

  void testResponse(const std::string &expect) const {
    std::for_each(this->final_responses_.begin(), this->final_responses_.end(),
                  [&expect](std::string response) { EXPECT_EQ(response, expect); });
  }

  /**
   * @brief Default Error Bodyを作成するhelper関数
   *
   * @param status_code
   * @return std::string
   */
  std::string createDefaultErrorBody(unsigned int status_code) const {
    const std::string webserv_error_page_tail = "<hr><center>webserv/1.0</center>\r\n</body>\r\n</html>\r\n";
    return *(HttpResponse::default_error_page_map_[status_code]) + webserv_error_page_tail;
  }

  /**
   * @brief Default Error Pageのサイズを計算する
   *
   * @param status_code
   * @return size_t
   */
  size_t calcDefaultBodySize(unsigned int status_code) const {
    std::string error_page = createDefaultErrorBody(status_code);
    return error_page.size();
  }

  void testDate(const std::string &actual) const {
    testDateFormat(actual);
    testDateTimeWithinRange(actual, 5);
  }

  void testDateFormat(const std::string &actual) const {
    std::regex pattern("^\\w{3}, \\d{2} \\w{3} \\d{4} \\d{2}:\\d{2}:\\d{2} GMT$");
    EXPECT_TRUE(std::regex_match(actual, pattern));
  }

  /**
   * @brief
   *
   * @param actual
   * @param acceptable_diff 許容する時間の差異
   */
  void testDateTimeWithinRange(const std::string &actual, int acceptable_diff) const {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::tm *gmt = std::gmtime(&current_time);

    // 文字列から時刻をパースして、time_t 型に変換する
    std::tm t = {};
    std::istringstream ss(actual);
    // Date: Tue, 23 Apr 2024 07:15:28 GMT
    ss >> std::get_time(&t, "%a, %d %b %Y %H:%M:%S");
    if (ss.fail()) {
      err("Date: format error");
      return;
    }
    std::time_t time_actual = std::mktime(&t);

    // 時刻の差を計算
    // std::cerr << "gmt   =" << std::mktime(gmt) << "\n";
    // std::cerr << "actual=" << time_actual << "\n";
    double diff = std::difftime(std::mktime(gmt), time_actual);

    // 差が許容範囲内であれば true を返す
    EXPECT_TRUE(std::abs(diff) <= acceptable_diff);
  }

  /**
   * @brief final responseを作成するhelper関数
   *
   * @param status_code_line
   * @return std::string
   */
  std::string createResponse(const std::string &status_code_line) const {
    const HttpResponse response = this->responses_[0];
    const string_map_case_insensitive headers = response.headers_;

    std::string res = "HTTP/1.1 ";
    res += (status_code_line + "\r\n");
    for (string_map_case_insensitive::const_iterator it = headers.begin(); it != headers.end(); ++it)
      res += (toTitleCase(it->first) + ": " + it->second + "\r\n");
    res += "\r\n";
    res += response.body_;
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

  int sockets_[2];
  const std::string conf_path_;
  std::vector<config::REQUEST_METHOD> methods_; // testするmethod
  ConfigHandler config_handler_;
  HttpRequest request_;
  std::vector<TiedServer> tied_servers_;
  std::vector<HttpResponse> responses_;
  std::vector<std::string> final_responses_;
};
}  // namespace test

#endif