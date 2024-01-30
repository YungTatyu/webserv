#include "CGIHandler.hpp"

// ./cgi/script.hpp or ./cgi/scirpt.hpp?aaa ./cgi/script.hpp?
// or ./a.out
bool CGIHandler::isCGI( std::string& requestURI )
{
	return CGIHandler::isPHPExtension(requestURI) || CGIHandler::isExecutable(requestURI.c_str());
}

std::string CGIHandler::getQueryString( std::string& uri )
{
	std::size_t pos = uri.find("?");
	if ( pos != std::string::npos )
		return uri.substr( pos + 1 );
	return "";
}

std::string CGIHandler::getScriptPath( std::string& uri )
{
	return uri.substr(0, uri.find("?"));
}

bool CGIHandler::isExecutable(const char* filename)
{
	struct stat sbuf;
	if ( stat(filename, &sbuf) < 0 )
		return false;
	return S_ISREG(sbuf.st_mode) && (S_IXUSR & sbuf.st_mode);
}

bool CGIHandler::isPHPExtension(const std::string& filename)
{
	std::string phpExt = ".php";
	if ( filename.length() < phpExt.length() )
		return false;
	return std::equal( phpExt.begin(), phpExt.end(), filename.end() - phpExt.length() );
}

std::vector<std::string> CGIHandler::split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
		tokens.push_back(token);
	return tokens;
}

std::string CGIHandler::get_command_path(const std::string& command)
{
	char* path = std::getenv("PATH");
	if (path == NULL)
		return "";
	std::vector<std::string> directories = split(path, ':');
	for (size_t i = 0; i < directories.size(); ++i)
	{
		std::string command_path = directories[i] + "/" + command;
		if (access(command_path.c_str(), X_OK) == 0)
		    return command_path;
	}
	return "";
}

std::string CGIHandler::executeCGI( std::string& uri, std::string& query )
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
		setenv("QUERY_STRING", query.c_str(), 1);
		if ( CGIHandler::isPHPExtension(uri) )
		{
			char *cmd[] = {const_cast<char *>("php"), const_cast<char *>(uri.c_str()), NULL};
			execve( CGIHandler::get_command_path("php").c_str(), cmd, environ );
		}
		else
		{
			// c, c++の実行
			char *cmd[] = {const_cast<char *>(uri.c_str()), NULL};
			execve(uri.c_str(), cmd, environ);
		}
		std::exit(1);
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

