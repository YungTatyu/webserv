#include <iostream>
#include <string.h>
#include "conf.hpp"
#include "Main.hpp"
#include "WebServer.hpp"
#include "ConfigHandler.hpp"

int main(int ac, char *av[])
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " [config_file_name]" << std::endl;
		return 1;
	}

	const config::Main *config = config::initConfig(av[1]);
	if (config == NULL)
		return 1;

	WebServer server = WebServer(config);
	server.run();
	delete config;
	return 0;
}

