#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kUseridDomain = "userid_domain";

TEST(useridDomainTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/userid_domain/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_EQ(http.userid_domain.getName(), "domain");
	test::test_directives_set(http.directives_set, kUseridDomain, true);

	// server
	EXPECT_EQ(http.server_list[0].userid_domain.getName(), "123");
	test::test_directives_set(http.server_list[0].directives_set, kUseridDomain, true);

	// location
	EXPECT_EQ(http.server_list[0].location_list[0].userid_domain.getName(), "tachu");
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kUseridDomain, true);
}


TEST(useridDomainTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kUseridDomain, false);
	test::test_directives_set(http.server_list[0].directives_set, kUseridDomain, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kUseridDomain, false);
}
