#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const std::vector<config::ErrorPage> &error_page_list,
                const std::vector<std::vector<unsigned int>> &expect_codes,
                const std::vector<long> &expect_response, const std::vector<std::string> &expect_uri) {
  int ei = 0;  // error page index
  std::for_each(error_page_list.begin(), error_page_list.end(),
                [&ei, &expect_codes, &expect_response, &expect_uri](config::ErrorPage error_page) {
                  (void)error_page;
                  const std::set<unsigned int> &code_list = error_page.getCodeList();
                  const std::vector<unsigned int> &expect_code = expect_codes[ei];
                  int ci = 0;  // code list index

                  // test code_list
                  std::for_each(expect_code.begin(), expect_code.end(), [&code_list, &ci](unsigned int code) {
                    // EXPECT_EQ(code, expect_code[ci]);
                    EXPECT_NE(code_list.find(code), code_list.end());
                    ++ci;
                  });

                  // test response
                  EXPECT_EQ(error_page.getResponse(), expect_response[ei]);

                  // test uri
                  EXPECT_EQ(error_page.getUri(), expect_uri[ei]);
                  ++ei;
                });
}
}  // namespace test

const std::string kErrorPage = "error_page";

TEST(ErrorPageTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/error_page/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  // http
  test::test_value(http.error_page_list, {{300, 599}, {301, 598}, {300, 301, 302, 303, 598}}, {-1, -1, -1},
                   {"error1", "error2", "error3"});
  test::test_directives_set(http.directives_set, kErrorPage, true);

  // server
  test::test_value(server_list[0].error_page_list, {{400, 500}, {450, 550}, {401, 501}, {302}},
                   {922337203685477586, 922337203685477587, 0, -1},
                   {"response1", "response2", "response3", "response4"});
  test::test_directives_set(server_list[0].directives_set, kErrorPage, true);

  test::test_value(server_list[0].location_list[0].error_page_list,
                   {{400, 401, 402, 403, 404, 405}, {500, 501, 502, 503, 504, 505}}, {-1, -1},
                   {"=0", "=922337203685477588"});
  test::test_directives_set(server_list[0].location_list[0].directives_set, kErrorPage, true);
}

TEST(ErrorPageTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http;
  const std::vector<config::Server> &server_list = http.server_list;

  test::test_directives_set(http.directives_set, kErrorPage, false);
  test::test_directives_set(server_list[0].directives_set, kErrorPage, false);
  test::test_directives_set(server_list[0].location_list[0].directives_set, kErrorPage, false);
}
