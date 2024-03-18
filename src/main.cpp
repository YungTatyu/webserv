#include <iostream>
#include <string.h>
#include "conf.hpp"
#include "Main.hpp"
#include "WebServer.hpp"
#include "ConfigHandler.hpp"
#include "LogFd.hpp"

//std::map<int, std::string> HttpResponse::status_line_map_;
//std::map<int, const std::string*> HttpResponse::default_error_page_map_;

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
	config::terminateLogFds(config);
	delete config;
	return 0;
}

