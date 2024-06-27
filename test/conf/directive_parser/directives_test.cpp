#include "directives_test.hpp"

#include <algorithm>
#include <string>
#include <vector>

void test::test_directives_set(const std::set<std::string> &directives_set, const std::string &directive_name,
                               bool expect_found) {
  if (!expect_found) {
    EXPECT_TRUE(directives_set.find(directive_name) == directives_set.end());
    return;
  }
  EXPECT_TRUE(directives_set.find(directive_name) != directives_set.end());
}

void test::test_value(const std::vector<config::AllowDeny> &list,
                      const std::vector<std::string> &address_expects,
                      const std::vector<config::ACCESS_DIRECTIVE> &access_expects) {
  int i = 0;
  std::for_each(list.begin(), list.end(), [&i, &address_expects, &access_expects](config::AllowDeny access) {
    EXPECT_EQ(access.getAddress(), address_expects[i]);
    EXPECT_EQ(access.getAccessDirective(), access_expects[i]);
    ++i;
  });
}
