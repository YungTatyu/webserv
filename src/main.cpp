#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include "Parser.hpp"

int	main(void)
{
	std::cout << "Hello webserv" << std::endl;

	std::vector<config::Token>	tokens_;
	config::Parser	parser(tokens_, "filepath");

	char **arg;

	arg = (char **)malloc(sizeof(char *) * 2);
	arg[0] = strdup("cgi_exe");
	arg[1] = NULL;
	execve("cgi/cgi_exe", arg, NULL);
	return (0);
}
