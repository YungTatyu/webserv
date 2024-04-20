#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include "conf.hpp"
#include "Main.hpp"
#include "directives_test.hpp"

const std::string	kWorkerConnections = "worker_connections";

TEST(WorkerConnectionsTest, max)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/worker_connections/1.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.worker_connections.getWorkerConnections(), 9223372036854775807);
	test::test_directives_set(events.directives_set, kWorkerConnections, true);
}

TEST(WorkerConnectionsTest, random)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/worker_connections/2.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.worker_connections.getWorkerConnections(), 9223372036854775806);
	test::test_directives_set(events.directives_set, kWorkerConnections, true);
}

TEST(WorkerConnectionsTest, min)
{
	const config::Main	*config = config::initConfig("test/conf/directive_parser/worker_connections/3.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	EXPECT_EQ(events.worker_connections.getWorkerConnections(), 2);
	test::test_directives_set(events.directives_set, kWorkerConnections, true);
}



TEST(WorkerConnectionsTest, notFound) {
		
	const config::Main	*config = config::initConfig("test/conf/directive_parser/only_context.conf");
	ASSERT_NE(config, nullptr);

	const config::Http	&http = config->http;
	const config::Events	&events = config->events;
	const std::vector<config::Server>	&server_list = http.server_list;

	test::test_directives_set(events.directives_set, kWorkerConnections, false);
}
