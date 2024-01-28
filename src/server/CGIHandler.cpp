#include "CGIHandler.hpp"

bool CGIHandler::isCGI( std::string& requestURI)
{
	std::size_t pos = requestURI.find("?");
	if ( pos != std::string::npos )
		return ( requestURI.substr( 0, pos ) == "./cgi" );
	else
		return ( requestURI == "./cgi" );
}

std::string CGIHandler::executeCGI( std::string& uri )
{
	int pipefd[2];
	std::string buffer;
	std::string result;
	int status;
	extern char **environ;

	pipe( pipefd );
	if ( fork() == 0 )
	{
		close( pipefd[READ] );
		dup2( pipefd[WRITE], STDOUT_FILENO );
		close( pipefd[WRITE] );
		std::size_t pos = uri.find("?");
		if ( pos != std::string::npos )
			setenv("QUERY_STRING", uri.substr( pos + 1 ).c_str(), 1);
		else
			setenv("QUERY_STRING", "", 1);
		char *cmd[] = {const_cast<char *>("php"), const_cast<char *>("cgi/script.php"), NULL};
		execve("/opt/homebrew/bin/php", cmd, environ);
	}
	else
	{
		wait(&status);
		close ( pipefd[WRITE] );
		std::string buffer(1024, '\0');
		read(pipefd[READ], &buffer[0], buffer.size());
		result = buffer;
		close ( pipefd[READ] );
	}
	return result;
}

