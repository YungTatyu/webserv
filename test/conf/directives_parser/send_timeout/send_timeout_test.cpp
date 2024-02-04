#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kSendTimeout = "send_timeout";

namespace test
{
void	test_st_value(const std::vector<config::Location> &list, const std::vector<unsigned long> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::Location location){
		EXPECT_EQ(location.send_timeout.getTime().time_in_ms_, expect[i]);
		test::test_directives_set(location.directives_set, kSendTimeout, true);
		++i;
	});
}
} // namespace test


TEST(sendTimeoutTest, max)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/send_timeout/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_EQ(http.send_timeout.getTime().time_in_ms_, LONG_MAX);
	test::test_directives_set(http.directives_set, kSendTimeout, true);

	// server
	EXPECT_EQ(http.server_list[0].send_timeout.getTime().time_in_ms_, LONG_MAX);
	test::test_directives_set(http.server_list[0].directives_set, kSendTimeout, true);

	// location
	for (size_t i = 0; i < http.server_list[0].location_list.size(); i++)
	{
		EXPECT_EQ(http.server_list[0].location_list[i].send_timeout.getTime().time_in_ms_, LONG_MAX);
		test::test_directives_set(http.server_list[0].location_list[i].directives_set, kSendTimeout, true);
	}
}

TEST(SendTimeoutTest, zero)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/send_timeout/2.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_EQ(http.send_timeout.getTime().time_in_ms_, 0);
	test::test_directives_set(http.directives_set, kSendTimeout, true);

	// server
	EXPECT_EQ(http.server_list[0].send_timeout.getTime().time_in_ms_, 0);
	test::test_directives_set(http.server_list[0].directives_set, kSendTimeout, true);

	// location
	for (size_t i = 0; i < http.server_list[0].location_list.size(); i++)
	{
		EXPECT_EQ(http.server_list[0].location_list[i].send_timeout.getTime().time_in_ms_, 0);
		test::test_directives_set(http.server_list[0].location_list[i].directives_set, kSendTimeout, true);
	}
}

TEST(SendTimeoutTest, random)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/keepalive_timeout/3.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_EQ(http.send_timeout.getTime().time_in_ms_, 1);
	test::test_directives_set(http.directives_set, kSendTimeout, true);

	// server
	EXPECT_EQ(http.server_list[0].send_timeout.getTime().time_in_ms_, 2000);
	test::test_directives_set(http.server_list[0].directives_set, kSendTimeout, true);

	// location
	test::test_st_value(http.server_list[0].location_list, {2 * 1000, 2 * 1000 * 60, 2 * 1000 * 60 * 60, 2 * 1000 * 60 * 60 * 24});
}

TEST(SendTimeoutTest, notFound) {
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kSendTimeout, false);
}
