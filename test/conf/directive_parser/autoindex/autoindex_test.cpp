#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

namespace test
{
void	test_value(const std::vector<config::Server> &list, const std::vector<bool> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::Server server){
		EXPECT_EQ(server.autoindex.getIsAutoindexOn(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kAutoindex = "autoindex";

TEST(autoindexTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/autoindex/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_EQ(http.autoindex.getIsAutoindexOn(), true);
	test::test_directives_set(http.directives_set, kAutoindex, true);

	// server
	test::test_value(http.server_list, {false, true});
	test::test_directives_set(http.server_list[0].directives_set, kAutoindex, true);
	test::test_directives_set(http.server_list[1].directives_set, kAutoindex, true);

	// location
	EXPECT_EQ(http.server_list[0].location_list[0].autoindex.getIsAutoindexOn(), true);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAutoindex, true);
}

TEST(autoindexTest, notFound) {
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kAutoindex, false);
	test::test_directives_set(http.server_list[0].directives_set, kAutoindex, false);
	test::test_directives_set(http.server_list[1].directives_set, kAutoindex, false);
	test::test_directives_set(http.server_list[2].directives_set, kAutoindex, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAutoindex, false);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kAutoindex, false);
}
