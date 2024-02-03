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
void	test_value(const std::vector<config::ErrorPage> &error_page_list, const std::vector<std::vector<unsigned int>> &expects)
{
	int	ei = 0;
	std::for_each(error_page_list.begin(), error_page_list.end(), [&ei, &expects](config::ErrorPage error_page){
		const std::vector<unsigned int>	&code_list = error_page.getCodeList();
		const std::vector<unsigned int>	&expect = expects[ei];
		int	ci = 0;
		std::for_each(code_list.begin(), code_list.end(), [&ci, &expect](unsigned int code){
			EXPECT_EQ(code, expect[ci]);
			++ci;
		});
		++ei;
	});
}
} // namespace test

const std::string	kErrorPage = "error_page";

TEST(ErrorPageTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/error_page/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	test::test_value(http.error_page_list, {{300, 599}, {301, 598}, {300, 301, 302, 303, 598}});

}

TEST(ErrorPageTest, notFound) {
		
	const config::Main	*config = config::init_config("test/conf/directives_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kErrorPage, false);
	test::test_directives_set(http.server_list[0].directives_set, kErrorPage, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kErrorPage, false);

}
