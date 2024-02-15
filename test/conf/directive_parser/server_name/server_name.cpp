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
void	test_value(const std::vector<config::ServerName> &list, const std::vector<std::string> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::ServerName server_name){
		EXPECT_EQ(server_name.getFile(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kServerName = "server_name";

TEST(ServerNameTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directive_parser/server_name/1.conf");
	ASSERT_NE(config, nullptr);
	EXPECT_TRUE(false); // server_nameの設計変更につき、テスト一旦保留

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// server
	test::test_value(http.server_list[0].server_name_list, {"path/to/file1", "path/to/file2"});
	test::test_value(http.server_list[1].server_name_list, {"server2path1", "server2path2"});
	test::test_directives_set(http.server_list[0].directives_set, kServerName, true);
	test::test_directives_set(http.server_list[1].directives_set, kServerName, true);

}

TEST(ServerNameTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kServerName, false);
	test::test_directives_set(http.server_list[0].directives_set, kServerName, false);
	test::test_directives_set(http.server_list[1].directives_set, kServerName, false);
}
