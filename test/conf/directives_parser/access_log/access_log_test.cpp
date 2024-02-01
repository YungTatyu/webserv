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
void	test_value(const std::vector<config::AccessLog> &list, const std::vector<std::string> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kAccessLog = "access_log";

TEST(accesslogTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/access_log/1.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	test::test_value(http.access_log_list, {"/tmp", "/tmp/tmp"});
	test::test_directives_set(http.directives_set, kAccessLog, true);

	// server
	test::test_value(http.server_list[0].access_log_list, {"path/to/file1", "path/to/file2"});
	test::test_value(http.server_list[1].access_log_list, {"server2path1", "server2path2"});
	test::test_directives_set(http.server_list[0].directives_set, kAccessLog, true);
	test::test_directives_set(http.server_list[1].directives_set, kAccessLog, true);

	// location
	test::test_value(http.server_list[0].location_list[0].access_log_list, {
		"path1",
		"path2",
		"path3",
		"path4",
		"path5",
	});
	test::test_value(http.server_list[0].location_list[1].access_log_list, {"1", "2", "3"});
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kAccessLog, true);
}

TEST(accesslogTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kAccessLog, false);
	test::test_directives_set(http.server_list[0].directives_set, kAccessLog, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAccessLog, false);

}
