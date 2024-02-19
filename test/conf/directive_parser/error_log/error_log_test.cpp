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
void	test_value(const std::vector<config::ErrorLog> &list, const std::vector<std::string> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::ErrorLog error_log){
		EXPECT_EQ(error_log.getFile(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kErrorLog = "error_log";

TEST(ErrorLogTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/error_log/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// main
	test::test_value(config->error_log_list, {"/tmp", "/tmp/path"});
	test::test_directives_set(config->directives_set, kErrorLog, true);

	// http
	test::test_value(http.error_log_list, {"/", "/path/"});
	test::test_directives_set(http.directives_set, kErrorLog, true);

	// server
	test::test_value(http.server_list[0].error_log_list, {"path1", "path2", "path3"});
	test::test_directives_set(http.server_list[0].directives_set, kErrorLog, true);

	// location
	test::test_value(http.server_list[0].location_list[0].error_log_list, {"/server1", "/server2", "/server3"});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kErrorLog, true);
}

TEST(ErrorLogTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(config->directives_set, kErrorLog, false);
	test::test_directives_set(http.directives_set, kErrorLog, false);
	test::test_directives_set(http.server_list[0].directives_set, kErrorLog, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kErrorLog, false);

}
