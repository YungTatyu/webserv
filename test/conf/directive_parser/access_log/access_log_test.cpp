#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <sys/param.h>

#include "conf.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Main.hpp"
#include "directives_test.hpp"
#include "Utils.hpp"


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

config::Main	*initConfigTest( const std::string& file_path )
{
	char	absolute_path[MAXPATHLEN];

	// 絶対pathを取得
	if (realpath(file_path.c_str(), absolute_path) == NULL)
	{
		std::cerr << "webserv: [emerg] realpath() \"" << file_path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return NULL;
	}

	// file_path が存在するかどうか
	if (Utils::wrapperAccess(absolute_path, F_OK, true) == -1)
		return NULL;

	// file_path の読み取り権限があるかどうか
	if (Utils::wrapperAccess(absolute_path, R_OK, true) == -1)
		return NULL;

	// file_path がファイルかどうか確認する。
	if (!Utils::isFile(absolute_path))
	{
		std::cerr << "webserv: [crit] \"" << absolute_path << "\" is a directory" << std::endl;
		return NULL;
	}

	config::Lexer lexer(absolute_path);
	lexer.tokenize();
	const std::vector<config::Token>	&tokens = lexer.getTokens();

	config::Main	*config = new config::Main();
	config::Parser	parser(*config, tokens, absolute_path);
	if (!parser.parse())
	{
		delete config;
		return NULL;
	}
	return config;
}

} // namespace test

const std::string	kAccessLog = "access_log";

TEST(accesslogTest, allContext)
{
	const config::Main	*config = test::initConfigTest("test/conf/directive_parser/access_log/1.conf");
	ASSERT_NE(config, nullptr);

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
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAccessLog, true);
	test::test_directives_set(http.server_list[0].location_list[1].directives_set, kAccessLog, true);
}

TEST(accesslogTest, notFound) {
		
	const config::Main	*config = config::initConfig("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(http.directives_set, kAccessLog, false);
	test::test_directives_set(http.server_list[0].directives_set, kAccessLog, false);
	test::test_directives_set(http.server_list[1].directives_set, kAccessLog, false);
	test::test_directives_set(http.server_list[2].directives_set, kAccessLog, false);
	test::test_directives_set(http.server_list[0].location_list[0].directives_set, kAccessLog, false);
}
