#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kClientMaxBodySize = "client_max_body_size";

TEST(clientMaxBodySizeTest, longMax)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/client_max_body_size/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, LONG_MAX);
	test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}

TEST(clientMaxBodySizeTest, longMaxKilo)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/client_max_body_size/3.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, LONG_MAX / config::Size::kilobytes);
	test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}

TEST(clientMaxBodySizeTest, longMaxMega)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/client_max_body_size/4.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, LONG_MAX / config::Size::megabytes);
	test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}


TEST(clientMaxBodySizeTest, zero)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/client_max_body_size/2.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(http.client_max_body_size.getSize().size_in_bytes_, 0);
	test::test_directives_set(http.directives_set, kClientMaxBodySize, true);
}


TEST(clientMaxBodySizeTest, notFound) {
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kClientMaxBodySize, false);
}
