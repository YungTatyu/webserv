#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#include "conf.hpp"
#include "Main.hpp"


TEST(mainParserTest, allContexts)
{
	const config::Main	*config = config::init_config("test/conf/directives_parser/access_log/");
	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;
	int	i;



}