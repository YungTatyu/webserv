#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kDeny = "deny";

TEST(DenyTest, ipv4)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/deny/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;


	// http
	test::test_value(http.allow_deny_list, {"all", "192.168.1.0/24"}, {config::DENY, config::DENY});
	test::test_directives_set(http.directives_set, kDeny, true);

	// server
	test::test_value(http.server_list[0].allow_deny_list, {"87.65.43.21/32", "87.65.0.0/16"}, {config::DENY, config::DENY});
	test::test_directives_set(http.server_list[0].directives_set, kDeny, true);

	// location
	test::test_value(http.server_list[0].location_list[0].allow_deny_list, {
		"203.0.113.1/24",
		"45.33.32.156/28",
		"45.33.32.1/32",
		"45.33.32.1/31",
		"224.0.0.1/32",
	}, {
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY
	});
	test::test_value(http.server_list[0].location_list[1].allow_deny_list, {"192.0.2.2", "10.0.0.2"}, {config::DENY, config::DENY});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kDeny, true);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kDeny, true);

	// limit_except
	test::test_value(http.server_list[0].location_list[1].limit_except.allow_deny_list,
		{"112.85.90.68", "192.168.0.1", "0.0.0.0"},
		{config::DENY, config::DENY, config::DENY}
	);
	test::test_directives_set(http.server_list[0].location_list[1].limit_except.directives_set, kDeny, true);
}

TEST(DenyTest, ipv6)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/deny/2.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;


	// http
	test::test_value(http.allow_deny_list, {"3001:0db8::1", "fe81::2"}, {config::DENY, config::DENY});
	test::test_directives_set(http.directives_set, kDeny, true);

	// server
	test::test_value(http.server_list[0].allow_deny_list, {"::4", "::5", "all"}, {config::DENY, config::DENY, config::DENY});
	test::test_directives_set(http.server_list[0].directives_set, kDeny, true);

	// location
	test::test_value(http.server_list[0].location_list[0].allow_deny_list, {
		"3001:0db8::/32",
		"fe81::/1",
		"fc01::/7",
		"::/127",
		"abcd:ef01:2345:6789::/64",
		"::ffff",
		"FFFF::",
	}, {
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY
	});
	test::test_value(http.server_list[0].location_list[1].allow_deny_list, {
		"3001:db8:0:42:0:8a2e:370:7334",
		"::ffff:192.168.0.4",
		"FFFF::192.24.0.4",
	}, {
		config::DENY,
		config::DENY,
		config::DENY
	});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kDeny, true);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kDeny, true);

	// limit_except
	test::test_value(http.server_list[0].location_list[1].limit_except.allow_deny_list, {
		"3001:0db8::/128",
		"3001:DB8::fFFf:4/127",
		"3001:0db8:0000:0042:0000:8a2e:0370:7334/0",
		"::/0",
		"0001:0000:0000:0000:0000:0000:0000:0000/100",
	}, {
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY,
		config::DENY
	});
	test::test_directives_set(http.server_list[0].location_list[1].limit_except.directives_set, kDeny, true);
}


TEST(DenyTest, notFound) {
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	test::test_directives_set(http.directives_set, kDeny, false);

	// server
	test::test_directives_set(http.server_list[0].directives_set, kDeny, false);
	test::test_directives_set(http.server_list[1].directives_set, kDeny, false);
	test::test_directives_set(http.server_list[2].directives_set, kDeny, false);

	// location
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kDeny, false);
	test::test_directives_set(http.server_list[1].location_list[0].directives_set, kDeny, false);

	// limit_except
	test::test_directives_set(http.server_list[0].location_list[0].limit_except.directives_set, kDeny, false);
}
