#include <gtest/gtest.h>
#include "AllowDeny.hpp"

namespace test
{	
void	test_directives_set(const std::set<std::string> &directives_set, const std::string &directive_name, bool expect_found);
void	test_value(
			const std::vector<config::AllowDeny> &list,
			const std::vector<std::string> &address_expects,
			const std::vector<config::ACCESS_DIRECTIVE> &access_expects
		);

} // namespace test
