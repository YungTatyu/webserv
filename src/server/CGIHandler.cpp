#include "CGIHandler.hpp"
#include "Utils.hpp"
#include "SysCallWrapper.hpp"
#include <cstdlib>

// ./cgi/script.hpp or ./cgi/scirpt.hpp?aaa ./cgi/script.hpp?
// or ./a.out
bool CGIHandler::isCGI( std::string& requestURI )
{
	return Utils::isPHPExtension(requestURI) || Utils::isExecutable(requestURI.c_str());
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

std::string CGIHandler::getCommandPath( const std::string& command )
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

cgi::CGIHandler::CGIHandler() : cgi_process_id_(-1) {}

cgi::CGIHandler::~CGIHandler() {}

/**
 * @brief cgi実行fileか
 * cgi実行ファイルの拡張子
 * 1. *.php
 * 2. *.cig
 * 3. *.py
 * 
 * @param cgi_path 
 * @return true 
 * @return false 
 */
bool cgi::CGIHandler::isCgi(const std::string& cgi_path)
{
	int pipefd[2];
	std::string buffer;
	std::string result;
	int status;
	// extern char **environ;

	if ( SysCallWrapper::Pipe( pipefd ) == -1 )
		return "CGI Execution Failed";
	fcntl(pipefd[READ], F_SETFL, O_NONBLOCK);
	fcntl(pipefd[WRITE], F_SETFL, O_NONBLOCK);
	pid_t pid = SysCallWrapper::Fork();
	if ( pid == -1 )
	{
		close ( pipefd[WRITE] );
		close ( pipefd[READ] );
		return "CGI Execution Failed";
	}
	if ( pid == 0 )
	{
		close( pipefd[READ] );
		SysCallWrapper::Dup2( pipefd[WRITE], STDOUT_FILENO ); // Exit in Wrapper on fail.
		close( pipefd[WRITE] );
		// setenv("QUERY_STRING", query.c_str(), 1);
		std::string env = "QUERY_STRING=" + query;
		char *environ[] = {const_cast<char *>(env.c_str()), NULL};

		if ( Utils::isPHPExtension(uri) )
		{
			char *cmd[] = {const_cast<char *>("php"), const_cast<char *>(uri.c_str()), NULL};
			execve( CGIHandler::getCommandPath("php").c_str(), cmd, environ );
		}
		else
		{
			// c, c++の実行
			char *cmd[] = {const_cast<char *>(uri.c_str()), NULL};
			execve(uri.c_str(), cmd, environ);
		}
		std::exit( EXIT_FAILURE );
	}
	else
	{
		time_t start_time = time(NULL);
		int timeout = 10;
		while ((time(NULL) - start_time) < timeout)
		{
			if (waitpid(pid, &status, WNOHANG) > 0)
			{
				if (WIFEXITED(status))
				{
					std::string buffer(1024, '\0');
					read(pipefd[READ], &buffer[0], buffer.size());
					result = buffer;
					break ;
				}
			}
		}

		kill( pid, SIGKILL );
		waitpid( pid, NULL, 0 );

		close ( pipefd[WRITE] );
		close ( pipefd[READ] );
	}
	return result;
}

