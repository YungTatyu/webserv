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
void	test_value(
	const config::TryFiles &try_files,
	const std::vector<std::string> &expect_files,
	const std::string &expect_uri,
	const int &expect_code
)
{
	int	i = 0;
	const std::vector<std::string>	&file_list = try_files.getFileList();
	std::for_each(file_list.begin(), file_list.end(),
		[&i, &expect_files](std::string file){
		EXPECT_EQ(file, expect_files[i]);
		++i;
	});

	// test uri
	EXPECT_EQ(try_files.getUri(), expect_uri);

	// test code
	EXPECT_EQ(try_files.getCode(), expect_code);
}
} // namespace test

const std::string	kTryFiles = "try_files";

TEST(TryFilesTest, allContext)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/try_files/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// server	
	test::test_value(http.server_list[0].try_files, {"file1", "file2", "file3"}, "uri", -1);
	test::test_directives_set(http.server_list[0].directives_set, kTryFiles, true);

	// location
	test::test_value(http.server_list[0].location_list[0].try_files, {"/index.html"}, "test", -1);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kTryFiles, true);

	test::test_value(http.server_list[0].location_list[1].try_files, {"/main.html"}, "1000", -1);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kTryFiles, true);

	test::test_value(http.server_list[0].location_list[2].try_files, {"/file1", "/file2", "/file3"}, "", 0);
	test::test_directives_set(http.server_list[0].location_list[2].directives_set, kTryFiles, true);

	test::test_value(http.server_list[0].location_list[3].try_files, {"/file_1", "/file_2"}, "", 999);
	test::test_directives_set(http.server_list[0].location_list[3].directives_set, kTryFiles, true);

	test::test_value(http.server_list[0].location_list[4].try_files, {"file"}, "", 1);
	test::test_directives_set(http.server_list[0].location_list[4].directives_set, kTryFiles, true);

}

TEST(TryFilesTest, notFound) {
		
	const config::Main	*config = config::initConfig("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kTryFiles, false);
	test::test_directives_set(http.server_list[0].directives_set, kTryFiles, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kTryFiles, false);

}
