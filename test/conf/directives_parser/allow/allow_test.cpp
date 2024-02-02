#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

namespace test
{
void	test_value(const std::vector<config::Allow> &list, const std::vector<std::string> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::Allow allow){
		EXPECT_EQ(allow.getAddress(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kAllow = "allow";

TEST(allowTest, ipv4)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/allow/1.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;


	// http
	test::test_value(http.allow_list, {"all", "1.1.1.1/30"});
	test::test_directives_set(http.directives_set, kAllow, true);

	// server
	test::test_value(http.server_list[0].allow_list, {"87.65.43.21/32", "87.65.0.21/2"});
	test::test_directives_set(http.server_list[0].directives_set, kAllow, true);

	// location
	test::test_value(http.server_list[0].location_list[0].allow_list, {
		"203.0.113.1/10",
		"45.33.32.156/30",
		"45.33.32.1/0",
		"45.33.32.1/1",
		"224.0.0.1/31",
	});
	test::test_value(http.server_list[0].location_list[1].allow_list, {"192.0.2.1", "10.0.0.1"});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAllow, true);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kAllow, true);

	// limit_except
	test::test_value(http.server_list[0].location_list[1].limit_except.allow_list, {"112.85.90.67", "255.255.255.255", "0.0.0.0"});
	test::test_directives_set(http.server_list[0].location_list[1].limit_except.directives_set, kAllow, true);
}

TEST(allowTest, notFound) {
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	test::test_directives_set(http.directives_set, kAllow, false);

	// server
	test::test_directives_set(http.server_list[0].directives_set, kAllow, false);
	test::test_directives_set(http.server_list[1].directives_set, kAllow, false);
	test::test_directives_set(http.server_list[2].directives_set, kAllow, false);

	// location
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAllow, false);
	test::test_directives_set(http.server_list[1].location_list[0].directives_set, kAllow, false);

	// limit_except
	test::test_directives_set(http.server_list[0].location_list[0].limit_except.directives_set, kAllow, false);
}
