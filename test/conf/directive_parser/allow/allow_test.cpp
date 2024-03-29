#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kAllow = "allow";

TEST(allowTest, ipv4)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/allow/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;


	// http
	test::test_value(http.allow_deny_list, {"all", "1.1.1.1/30"}, {config::ALLOW, config::ALLOW});
	test::test_directives_set(http.directives_set, kAllow, true);

	// server
	test::test_value(http.server_list[0].allow_deny_list, {"87.65.43.21/32", "87.65.0.21/2"}, {config::ALLOW, config::ALLOW});
	test::test_directives_set(http.server_list[0].directives_set, kAllow, true);

	// location
	test::test_value(http.server_list[0].location_list[0].allow_deny_list, {
		"203.0.113.1/10",
		"45.33.32.156/30",
		"45.33.32.1/0",
		"45.33.32.1/1",
		"224.0.0.1/31"
	}, {
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW
	});
	test::test_value(http.server_list[0].location_list[1].allow_deny_list, {"192.0.2.1", "10.0.0.1"}, {config::ALLOW, config::ALLOW});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAllow, true);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kAllow, true);

	// limit_except
	test::test_value(http.server_list[0].location_list[1].limit_except.allow_deny_list,
		{"112.85.90.67", "255.255.255.255", "0.0.0.0"},
		{config::ALLOW, config::ALLOW, config::ALLOW}
	);
	test::test_directives_set(http.server_list[0].location_list[1].limit_except.directives_set, kAllow, true);
}

TEST(allowTest, ipv6)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/allow/2.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;


	// http
	test::test_value(http.allow_deny_list, {"2001:0db8::1", "fe80::2"}, {config::ALLOW, config::ALLOW});
	test::test_directives_set(http.directives_set, kAllow, true);

	// server
	test::test_value(http.server_list[0].allow_deny_list, {"::3", "::5", "all"}, {config::ALLOW, config::ALLOW, config::ALLOW});
	test::test_directives_set(http.server_list[0].directives_set, kAllow, true);

	// location
	test::test_value(http.server_list[0].location_list[0].allow_deny_list, {
		"2001:0db8::/32",
		"fe80::/1",
		"fc00::/7",
		"::/128",
		"abcd:ef01:2345:6789::/64"
	}, {
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW
	});
	test::test_value(http.server_list[0].location_list[1].allow_deny_list, {
		"2001:db8:0:42:0:8a2e:370:7334",
		"::ffff:192.168.0.1",
		"FFFF::192.24.0.1",
		"21::12.24.0.1/128",
		"aaaa::12.24.0.1/128"
	}, {
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW
	});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAllow, true);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kAllow, true);

	// limit_except
	test::test_value(http.server_list[0].location_list[1].limit_except.allow_deny_list, {
		"2001:0db8::/128",
		"2001:DB8::fFFf:1/127",
		"2001:0db8:0000:0042:0000:8a2e:0370:7334/0",
		"0:0:0:0:0:0:0:0/0",
		"0000:0000:0000:0000:0000:0000:0000:0000/100"
	}, {
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW,
		config::ALLOW
	});
	test::test_directives_set(http.server_list[0].location_list[1].limit_except.directives_set, kAllow, true);
}


TEST(allowTest, notFound) {
	const config::Main	*config = config::initConfig("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

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
