#include <gtest/gtest.h>
#include "conf.hpp"
#include "Main.hpp"

namespace test
{
} // namespace test


const static std::string	HTTP = "http";
const static std::string	EVENTS = "events";
const static std::string	SERVER = "server";
const static std::string	LOCATION = "location";
const static std::string	LIMIT_EXCEPT = "limit_except";

TEST(mainParserTest, allContexts)
{
	const config::Main	*config = config::init_config("test/conf/conf_files/valid/12.conf");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	// http
	EXPECT_NE(config->directives_set.find(HTTP), config->directives_set.end());

	// events
	EXPECT_NE(config->directives_set.find(EVENTS), config->directives_set.end());

	// server
	EXPECT_NE(http.directives_set.find(SERVER), http.directives_set.end());

	EXPECT_NE(server_list[0].directives_set.find(LOCATION), server_list[0].directives_set.end());
	EXPECT_NE(server_list[1].directives_set.find(LOCATION), server_list[1].directives_set.end());
	EXPECT_NE(server_list[2].directives_set.find(LOCATION), server_list[2].directives_set.end());
	EXPECT_EQ(server_list[3].directives_set.find(LOCATION), server_list[3].directives_set.end());

	EXPECT_EQ(server_list.size(), 4);

	// location
	EXPECT_EQ(server_list[0].location_list[0].uri, "path");
	EXPECT_EQ(server_list[1].location_list[0].uri, "");
	EXPECT_EQ(server_list[2].location_list[0].uri, "path1");
	EXPECT_EQ(server_list[2].location_list[1].uri, "path2");
	EXPECT_EQ(server_list[2].location_list[2].uri, "path3");
	EXPECT_EQ(server_list[2].location_list[3].uri, "path4");
	EXPECT_EQ(server_list[2].location_list[4].uri, "path5");
	EXPECT_EQ(server_list[2].location_list[5].uri, "path6");
	EXPECT_EQ(server_list[2].location_list[6].uri, "path7");

	EXPECT_EQ(server_list[0].location_list.size(), 1);
	EXPECT_EQ(server_list[1].location_list.size(), 1);
	EXPECT_EQ(server_list[2].location_list.size(), 7);
	EXPECT_EQ(server_list[3].location_list.size(), 0);

	// limit except
	EXPECT_NE(
		server_list[0].location_list[0].directives_set.find(LIMIT_EXCEPT),
		server_list[0].location_list[0].directives_set.end()
	);
	EXPECT_NE(
		server_list[1].location_list[0].directives_set.find(LIMIT_EXCEPT),
		server_list[1].location_list[0].directives_set.end()
	);
	for (size_t i = 0; i < server_list[2].location_list.size(); i++)
	{
		EXPECT_EQ(
			server_list[2].location_list[i].directives_set.find(LIMIT_EXCEPT),
			server_list[2].location_list[i].directives_set.end()
		);
	}

	EXPECT_NE(
		server_list[0].location_list[0].limit_except.excepted_methods.find(config::GET),
		server_list[0].location_list[0].limit_except.excepted_methods.end()
	);
	EXPECT_NE(
		server_list[0].location_list[0].limit_except.excepted_methods.find(config::POST),
		server_list[0].location_list[0].limit_except.excepted_methods.end()
	);
	EXPECT_NE(
		server_list[0].location_list[0].limit_except.excepted_methods.find(config::DELETE),
		server_list[0].location_list[0].limit_except.excepted_methods.end()
	);
	EXPECT_NE(
		server_list[0].location_list[0].limit_except.excepted_methods.find(config::HEAD),
		server_list[0].location_list[0].limit_except.excepted_methods.end()
	);
	
	EXPECT_EQ(
		server_list[1].location_list[0].limit_except.excepted_methods.find(config::GET),
		server_list[1].location_list[0].limit_except.excepted_methods.end()
	);
	EXPECT_NE(
		server_list[1].location_list[0].limit_except.excepted_methods.find(config::POST),
		server_list[1].location_list[0].limit_except.excepted_methods.end()
	);
	EXPECT_NE(
		server_list[1].location_list[0].limit_except.excepted_methods.find(config::DELETE),
		server_list[1].location_list[0].limit_except.excepted_methods.end()
	);
	EXPECT_EQ(
		server_list[1].location_list[0].limit_except.excepted_methods.find(config::HEAD),
		server_list[1].location_list[0].limit_except.excepted_methods.end()
	);

	EXPECT_EQ(server_list[0].location_list[0].limit_except.excepted_methods.size(), 4);
	EXPECT_EQ(server_list[1].location_list[0].limit_except.excepted_methods.size(), 2);
	EXPECT_EQ(server_list[2].location_list[0].limit_except.excepted_methods.size(), 0);

}
