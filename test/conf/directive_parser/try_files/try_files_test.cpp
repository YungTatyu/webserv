#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

namespace test {
void test_value(const config::TryFiles &try_files, const std::vector<std::string> &expect_files,
                const std::string &expect_uri, const int &expect_code) {
  int i = 0;
  const std::vector<std::string> &file_list = try_files.getFileList();
  std::for_each(file_list.begin(), file_list.end(), [&i, &expect_files](std::string file) {
    EXPECT_EQ(file, expect_files[i]);
    ++i;
  });

  // test uri
  EXPECT_EQ(try_files.getUri(), expect_uri);

  // test code
  EXPECT_EQ(try_files.getCode(), expect_code);
}
}  // namespace test

const std::string kTryFiles = "try_files";

TEST(TryFilesTest, allContext) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/try_files/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  // server
  test::test_value(server_list[0].try_files_, {"file1", "file2", "file3"}, "uri", -1);
  test::test_directives_set(server_list[0].directives_set_, kTryFiles, true);

  // location
  test::test_value(server_list[0].location_list_[0].try_files_, {"/index.html"}, "test", -1);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kTryFiles, true);

  test::test_value(server_list[0].location_list_[1].try_files_, {"/main.html"}, "1000", -1);
  test::test_directives_set(server_list[0].location_list_[1].directives_set_, kTryFiles, true);

  test::test_value(server_list[0].location_list_[2].try_files_, {"/file1", "/file2", "/file3"}, "", 0);
  test::test_directives_set(server_list[0].location_list_[2].directives_set_, kTryFiles, true);

  test::test_value(server_list[0].location_list_[3].try_files_, {"/file_1", "/file_2"}, "", 999);
  test::test_directives_set(server_list[0].location_list_[3].directives_set_, kTryFiles, true);

  test::test_value(server_list[0].location_list_[4].try_files_, {"file"}, "", 1);
  test::test_directives_set(server_list[0].location_list_[4].directives_set_, kTryFiles, true);
}

TEST(TryFilesTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Http &http = config->http_;
  const std::vector<config::Server> &server_list = http.server_list_;

  test::test_directives_set(http.directives_set_, kTryFiles, false);
  test::test_directives_set(server_list[0].directives_set_, kTryFiles, false);
  test::test_directives_set(server_list[0].location_list_[0].directives_set_, kTryFiles, false);
}
