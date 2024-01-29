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

std::string CGIHandler::getScriptPath( std::string& uri )
{
	std::size_t pos = uri.find("?");
	if ( pos != std::string::npos )
		return uri.substr( 2, pos - 2 );
	else
		return uri.substr(2); // ./cgi/script.phpなら./を削除
}

bool CGIHandler::isFileExistAndExecutable(const char* filename)
{
	std::ifstream ifile(filename);
	if (!filename)
		return false;

	struct stat sbuf;
	if ( stat(filename, &sbuf) < 0 )
		return false;

	return S_ISREG(sbuf.st_mode) && (S_IXUSR & sbuf.st_mode);
}

bool CGIHandler::isPHPExtension(const std::string& filename)
{
    const size_t phpExtLength = 4;

    if (filename.length() < phpExtLength)
        return false;
    std::string ext = filename.substr(filename.length() - phpExtLength);
    return ext == ".php";
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
		std::string scriptPath = CGIHandler::getScriptPath(uri);
		if ( CGIHandler::isPHPExtension(scriptPath) )
		{
			char *cmd[] = {const_cast<char *>("php"), const_cast<char *>(scriptPath.c_str()), NULL};
			if ( CGIHandler::isFileExistAndExecutable(scriptPath.c_str()) )
				execve("/opt/homebrew/bin/php", cmd, environ);
			else
				std::cout << "script not executable" << std::endl;
		}
		else
		{ // c, c++の実行
			char *cmd[] = {const_cast<char *>(scriptPath.c_str()), NULL};
			if ( CGIHandler::isFileExistAndExecutable(scriptPath.c_str()) )
				execve(scriptPath.c_str(), cmd, environ);
			else
				std::cout << "script not executable" << std::endl;
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

