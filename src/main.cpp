#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include "AccessLog.hpp"

int	main(void)
{
	std::cout << "Hello webserv" << std::endl;
	config::AccessLog	access_log;

	std::cout << access_log.getFile() << "\n";
	access_log.setFile("test");
	std::cout << access_log.getFile() << "\n";

	char **arg;

	arg = (char **)malloc(sizeof(char *) * 2);
	arg[0] = strdup("cgi_exe");
	arg[1] = NULL;
	execve("cgi/cgi_exe", arg, NULL);
	return (0);
}
