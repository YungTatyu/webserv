#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"


namespace test
{
void	test_value(
	const std::vector<config::Return> &return_list,
	const std::vector<int> &code_list,
	const std::vector<std::string> &url_list
)
{
	int	i = 0; // Return index
	std::for_each(return_list.begin(), return_list.end(),
		[&i, &code_list, &url_list](config::Return return_){
		// test code
		EXPECT_EQ(return_.getCode(), code_list[i]);

		// test url
		EXPECT_EQ(return_.getUrl(), url_list[i]);

		++i;
	});
}
} // namespace test

const std::string	kReturn = "return";

TEST(ReturnTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/return/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// location
	test::test_value(http.server_list[0].location_list[0].return_list,
		{301, -1, 999, 998, 0, 0},
		{"url", "https://www.tachu.com", "url999", "1000", "", ""}
	);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kReturn, true);
}

TEST(ReturnTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kReturn, false);
}
