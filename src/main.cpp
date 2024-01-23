#include <iostream>
#include <string.h>
#include "WebServer.hpp"
#include "conf.hpp"

int main(int ac, char *av[])
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " [config_file_name]" << std::endl;
		return 1;
	}

	if (!config::init_config(av[1]))
		return 1;

	WebServer server = WebServer();
	return 0;
}

