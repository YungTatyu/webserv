#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"


namespace test
{
void	test(std::vector<config::Alias> list, std::vector<std::string> expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect[i]);
		++i;
	});
}
void	test_directives_set(const std::set<std::string> &directives_set, const std::string &directive_name, bool expect_found)
{
	if (!expect_found) {
		EXPECT_EQ(directives_set.find(directive_name), directives_set.end());
		return;
	}
	EXPECT_NE(directives_set.find(directive_name), directives_set.end());
}

} // namespace test

TEST(aliasTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/alias/1.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	test::test(http.access_log_list, {"/tmp", "/tmp/tmp"});

	// server
	test::test(http.server_list[0].access_log_list, {"path/to/file1", "path/to/file2"});
	test::test(http.server_list[1].access_log_list, {"server2path1", "server2path2"});

	// location
	test::test(http.server_list[0].location_list[0].access_log_list, {
		"path1",
		"path2",
		"path3",
		"path4",
		"path5",
	});
	test::test(http.server_list[0].location_list[1].access_log_list, {"1", "2", "3"});
}

TEST(aliasTest, notFound) {
	
}
