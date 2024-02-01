#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

namespace test
{
void	test_value(std::vector<config::Allow> list, std::vector<std::string> expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::Allow allow){
		EXPECT_EQ(allow.getAddress(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kAllow = "allow";

TEST(aliasTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/allow/1.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// location
	test::test_value(http.server_list[0].location_list, {"/", "/tmp"});
	test::test_value(http.server_list[1].location_list, {"/path/to/file", "/file"});
	test::test_directives_set(http.server_list[0].directives_set, kAllow, true);
	test::test_directives_set(http.server_list[1].directives_set, kAllow, true);
}

TEST(aliasTest, notFound) {
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.server_list[0].directives_set, kAllow, false);
	test::test_directives_set(http.server_list[1].directives_set, kAllow, false);
}
