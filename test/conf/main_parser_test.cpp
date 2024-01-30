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
}
