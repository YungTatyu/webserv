#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kUse = "use";

TEST(UseTest, select)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/use/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.use.getConnectionMethod(), config::CONNECTION_METHOD::SELECT);
	test::test_directives_set(events.directives_set, kUse, true);
}

TEST(UseTest, poll)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/use/2.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.use.getConnectionMethod(), config::CONNECTION_METHOD::POLL);
	test::test_directives_set(events.directives_set, kUse, true);
}

TEST(UseTest, kqueue)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/use/3.conf");

	// 環境によって挙動が変わる
	#if defined(__APPLE__)
		ASSERT_NE(config, nullptr);
	#else
		ASSERT_EQ(config, nullptr);
		return;
	#endif

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.use.getConnectionMethod(), config::CONNECTION_METHOD::KQUEUE);
	test::test_directives_set(events.directives_set, kUse, true);
}

TEST(UseTest, epoll)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/use/4.conf");

	// 環境によって挙動が変わる
	#if defined(__APPLE__)
		ASSERT_EQ(config, nullptr);
		return;
	#else
		ASSERT_NE(config, nullptr);
	#endif

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.use.getConnectionMethod(), config::CONNECTION_METHOD::EPOLL);
	test::test_directives_set(events.directives_set, kUse, true);
}

TEST(UseTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(events.directives_set, kUse, false);
}
