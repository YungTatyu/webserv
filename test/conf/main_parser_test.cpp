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

	// http
	EXPECT_NE(config->directives_set.find(HTTP), config->directives_set.end());

	// events
	EXPECT_NE(config->directives_set.find(EVENTS), config->directives_set.end());

	// server
	EXPECT_NE(config->http.directives_set.find(SERVER), config->http.directives_set.end());

	EXPECT_NE(config->http.server_list[0].directives_set.find(LOCATION), config->http.server_list[0].directives_set.end());
	EXPECT_NE(config->http.server_list[1].directives_set.find(LOCATION), config->http.server_list[1].directives_set.end());
	EXPECT_NE(config->http.server_list[2].directives_set.find(LOCATION), config->http.server_list[2].directives_set.end());
	EXPECT_EQ(config->http.server_list[3].directives_set.find(LOCATION), config->http.server_list[3].directives_set.end());

	EXPECT_EQ(config->http.server_list.size(), 4);

	// location
	EXPECT_EQ(config->http.server_list[0].location_list[0].uri_, "path");
	EXPECT_EQ(config->http.server_list[1].location_list[0].uri_, "");
	EXPECT_EQ(config->http.server_list[2].location_list[0].uri_, "path1");
	EXPECT_EQ(config->http.server_list[2].location_list[1].uri_, "path2");
	EXPECT_EQ(config->http.server_list[2].location_list[2].uri_, "path3");
	EXPECT_EQ(config->http.server_list[2].location_list[3].uri_, "path4");
	EXPECT_EQ(config->http.server_list[2].location_list[4].uri_, "path5");
	EXPECT_EQ(config->http.server_list[2].location_list[5].uri_, "path6");
	EXPECT_EQ(config->http.server_list[2].location_list[6].uri_, "path7");

	EXPECT_EQ(config->http.server_list[0].location_list.size(), 1);
	EXPECT_EQ(config->http.server_list[1].location_list.size(), 1);
	EXPECT_EQ(config->http.server_list[2].location_list.size(), 7);
	EXPECT_EQ(config->http.server_list[3].location_list.size(), 0);

	// limit except
	EXPECT_NE(
		config->http.server_list[0].location_list[0].directives_set.find(LIMIT_EXCEPT),
		config->http.server_list[0].location_list[0].directives_set.end()
	);
	EXPECT_NE(
		config->http.server_list[1].location_list[0].directives_set.find(LIMIT_EXCEPT),
		config->http.server_list[1].location_list[0].directives_set.end()
	);
	for (size_t i = 0; i < config->http.server_list[2].location_list.size(); i++)
	{
		EXPECT_EQ(
			config->http.server_list[2].location_list[i].directives_set.find(LIMIT_EXCEPT),
			config->http.server_list[2].location_list[i].directives_set.end()
		);
	}

	EXPECT_NE(
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.find(config::GET),
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.end()
	);
	EXPECT_NE(
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.find(config::POST),
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.end()
	);
	EXPECT_NE(
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.find(config::DELETE),
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.end()
	);
	EXPECT_NE(
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.find(config::HEAD),
		config->http.server_list[0].location_list[0].limit_except.excepted_methods_.end()
	);
	
	EXPECT_EQ(
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.find(config::GET),
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.end()
	);
	EXPECT_NE(
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.find(config::POST),
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.end()
	);
	EXPECT_NE(
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.find(config::DELETE),
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.end()
	);
	EXPECT_EQ(
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.find(config::HEAD),
		config->http.server_list[1].location_list[0].limit_except.excepted_methods_.end()
	);

	EXPECT_EQ(config->http.server_list[0].location_list[0].limit_except.excepted_methods_.size(), 4);
	EXPECT_EQ(config->http.server_list[1].location_list[0].limit_except.excepted_methods_.size(), 2);
	EXPECT_EQ(config->http.server_list[2].location_list[0].limit_except.excepted_methods_.size(), 0);

}
