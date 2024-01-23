#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>

int	main(int argc, char *argv[])
{
	(void) argc;
	std::cerr << "Hello webserv " << argv[1] << std::endl;

	// std::vector<config::Token>	tokens_;
	// config::Parser	parser(tokens_, "filepath");

	// char **arg;

	// arg = (char **)malloc(sizeof(char *) * 2);
	// arg[0] = strdup("cgi_exe");
	// arg[1] = NULL;
	// execve("cgi/cgi_exe", arg, NULL);
	return (0);
}
