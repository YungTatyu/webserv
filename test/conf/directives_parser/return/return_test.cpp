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
	const std::vector<config::Return> &Return_list,
	const std::vector<std::string> &address_list,
	const std::vector<unsigned int> &port_list,
	const std::vector<bool> &default_server_list
)
{
	int	li = 0; // Return index
	std::for_each(Return_list.begin(), Return_list.end(),
		[&li, &address_list, &port_list, &default_server_list](config::Return Return){
		// test address
		EXPECT_EQ(Return.getAddress(), address_list[li]);

		// test port
		EXPECT_EQ(Return.getport(), port_list[li]);

		// test uri
		EXPECT_EQ(Return.getIsDefaultServer(), default_server_list[li]);
		++li;
	});
}
} // namespace test

const std::string	kReturn = "return";

TEST(ReturnTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/return/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// location
	test::test_value(http.server_list[0].location_list[0].Return_list,
		{"1.1.1.1", "127.1.1.1", "2.2.2.2", "3.3.3.3", "127.1.1.1", "127.1.1.1", "127.1.1.1", "127.1.1.1"},
		{80, 1, 2000, 80, 2400, 80, 1000, 65535},
		{false, false, false, true, true, false, false, true}
	);
	test::test_directives_set(http.server_list[0].directives_set, kReturn, true);
}

TEST(ReturnTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.server_list[0].directives_set, kReturn, false);
}
