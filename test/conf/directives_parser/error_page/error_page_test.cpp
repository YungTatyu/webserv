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
void	test_value(const std::vector<config::ErrorPage> &list, const std::vector<std::string> &expect)
{
	int	i = 0;
	std::for_each(list.begin(), list.end(), [&i, &expect](config::ErrorPage error_page){
		EXPECT_EQ(error_page.getFile(), expect[i]);
		++i;
	});
}
} // namespace test

const std::string	kErrorPage = "error_page";

TEST(ErrorPageTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/error_page/1.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// main
	test::test_value(config->error_page_list, {"/tmp", "/tmp/path"});
	test::test_directives_set(config->directives_set, kErrorPage, true);

	// http
	test::test_value(http.error_page_list, {"/", "/path/"});
	test::test_directives_set(http.directives_set, kErrorPage, true);

	// server
	test::test_value(http.server_list[0].error_page_list, {"path1", "path2", "path3"});
	test::test_directives_set(http.server_list[0].directives_set, kErrorPage, true);

	// location
	test::test_value(http.server_list[0].location_list[0].error_page_list, {"/server1", "/server2", "/server3"});
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kErrorPage, true);
}

TEST(ErrorPageTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(config->directives_set, kErrorPage, false);
	test::test_directives_set(http.directives_set, kErrorPage, false);
	test::test_directives_set(http.server_list[0].directives_set, kErrorPage, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kErrorPage, false);

}
