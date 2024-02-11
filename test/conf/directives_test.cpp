#include "directives_test.hpp"

void	test::test_directives_set(const std::set<std::string> &directives_set, const std::string &directive_name, bool expect_found)
{
	if (!expect_found) {
		EXPECT_EQ(directives_set.find(directive_name), directives_set.end());
		return;
	}
	EXPECT_NE(directives_set.find(directive_name), directives_set.end());
}
