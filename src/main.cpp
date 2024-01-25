#include <iostream>
#include <string.h>
#include "WebServer.hpp"
#include "conf.hpp"

int main(int ac, char *av[])
{
	if (
		(config::CONF_MAIN & 
		config::CONF_HTTP &
		config::CONF_HTTP_SERVER &
		// config::CONF_UNIQUE &
		// config::CONF_NOT_UNIQUE &
		config::CONF_NOARGS
& config::CONF_TAKE1
& config::CONF_TAKE2
& config::CONF_TAKE3
& config::CONF_TAKE4
& config::CONF_TAKE5
& config::CONF_TAKE6
& config::CONF_TAKE7
& config::CONF_1MORE
& config::CONF_2MORE
) != 0)
		return 1;
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " [config_file_name]" << std::endl;
		return 1;
	}

	if (!config::init_config(av[1]))
		return 1;

	WebServer server = WebServer();
	server.eventLoop();
	return 0;
}

