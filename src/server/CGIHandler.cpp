#include "CGIHandler.hpp"

bool CGIHandler::isCGI( std::string& requestURI)
{
	return ( requestURI.substr( 0, 5 ) == "./cgi" );
	// or cgi/script.hpp or cgi/scirpt.hpp?aaa cgi/script.hpp?
}
std::string CGIHandler::getQueryString( std::string& uri )
{
	std::size_t pos = uri.find("?");
	if ( pos != std::string::npos )
		return uri.substr( pos + 1 );
	else
		return "";
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
		setenv("QUERY_STRING", CGIHandler::getQueryString(uri).c_str(), 1);
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

