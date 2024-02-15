#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kUseridService = "userid_service";

TEST(useridServiceTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/userid_service/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_EQ(http.userid_service.getUseridService(), 9223372036854775807);
	test::test_directives_set(http.directives_set, kUseridService, true);

	// server
	EXPECT_EQ(http.server_list[0].userid_service.getUseridService(), 0);
	test::test_directives_set(http.server_list[0].directives_set, kUseridService, true);

	// location
	EXPECT_EQ(http.server_list[0].location_list[0].userid_service.getUseridService(), 9223372036854775806);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kUseridService, true);
}


TEST(useridServiceTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kUseridService, false);
	test::test_directives_set(http.server_list[0].directives_set, kUseridService, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kUseridService, false);
}
