#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"


namespace name
{
void	test(std::vector<config::AccessLog> list, std::vector<std::string> expect)
{
	
}
} // namespace name



TEST(accesslogTest, allContext)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/access_log/1.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;
	int	i;


	// http
	std::vector<std::string>	expect_http = {
		"/tmp",
		"/tmp/tmp",
	};

	i = 0;
	std::for_each(http.access_log_list.begin(), http.access_log_list.end(), [&i, &expect_http](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect_http[i]);
		++i;
	});


	// server
	std::vector<std::string>	expect_server1 = {
		"path/to/file1",
		"path/to/file2",
	};
	i = 0;
	std::for_each(http.server_list[0].access_log_list.begin(), http.server_list[0].access_log_list.end(), [&i, &expect_server1](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect_server1[i]);
		++i;
	});


	std::vector<std::string>	expect_server2 = {
		"server2path1",
		"server2path2",
	};
	i = 0;
	std::for_each(http.server_list[1].access_log_list.begin(), http.server_list[1].access_log_list.end(), [&i, &expect_server2](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect_server2[i]);
		++i;
	});


	std::vector<std::string>	expect_location1 = {
		"path1",
		"path2",
		"path3",
		"path4",
		"path5",
	};
	i = 0;
	std::for_each(http.server_list[0].location_list[0].access_log_list.begin(), http.server_list[0].location_list[0].access_log_list.end(), [&i, &expect_location1](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect_location1[i]);
		++i;
	});

	std::vector<std::string>	expect_location2 = {
		"1",
		"2",
		"3",
	};
	i = 0;
	std::for_each(http.server_list[0].location_list[1].access_log_list.begin(), http.server_list[0].location_list[1].access_log_list.end(), [&i, &expect_location2](config::AccessLog access_log){
		EXPECT_EQ(access_log.getFile(), expect_location2[i]);
		++i;
	});
}