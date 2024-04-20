#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

namespace test
{
void	test_value(const std::vector<config::Index> &list, const std::vector<std::string> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::Index index){
		EXPECT_EQ(index.getFile(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kIndex = "index";

TEST(indexTest, allContext)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/index/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	test::test_value(http.index_list, {"1.html", "2.html", "3.html", "1", "2", "3", "4", "5"});
	test::test_directives_set(http.directives_set, kIndex, true);

	// server
	test::test_value(http.server_list[0].index_list, {"index.html", "server1", "server2"});
	test::test_directives_set(http.server_list[0].directives_set, kIndex, true);

	// location
	test::test_value(http.server_list[0].location_list[0].index_list, {
		"location1", "location2", "location3",
		"index1.html", "index2.html", "index3.html", "index4.html"
	});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kIndex, true);
}

TEST(indexTest, notFound) {
	const config::Main	*config = config::initConfig("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.server_list[0].directives_set, kIndex, false);
	test::test_directives_set(http.server_list[1].directives_set, kIndex, false);
	test::test_directives_set(http.server_list[2].directives_set, kIndex, false);
}
