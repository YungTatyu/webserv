#include <gtest/gtest.h>
#include "conf.hpp"
#include "Main.hpp"

namespace test
{
} // namespace test


const static std::string	HTML = "html";
const static std::string	EVENTS = "events";
const static std::string	SERVER = "server";
const static std::string	LOCATION = "location";
const static std::string	LIMIT_EXCEPT = "limit_except";

TEST(mainParserTest, allContexts)
{
	const Main	*config = config::init_config("conf_files/valid/12.conf");

	// http
	EXPECT_NOT_EQ(config->directives_set.find(HTML), config->directives_set.end());
}
