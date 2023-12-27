/*
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>

int	main(void)
{
	std::cout << "Hello webserv" << std::endl;

	char **arg;

	arg = (char **)malloc(sizeof(char *) * 2);
	arg[0] = strdup("cgi_exe");
	arg[1] = NULL;
	execve("cgi/cgi_exe", arg, NULL);
	return (0);
}
*/

#include "EchoServer.hpp"

int main()
{
	EchoServer server = EchoServer();
	server.eventLoop();
	return 0;
}

