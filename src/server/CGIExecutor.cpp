#include "CGIExecutor.hpp"
#include "FileUtils.hpp"

#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <sstream>

cgi::CGIExecutor::CGIExecutor() {}

cgi::CGIExecutor::~CGIExecutor() {}

void	cgi::CGIExecutor::executeCgiScript(
	const HttpRequest& http_request,
	const std::string& script_path,
	const int socket
)
{
	prepareCgiExecution(http_request, script_path, socket);
	execve(
		this->script_path_.c_str(),
		const_cast<char *const *>(this->argv_.data()),
		const_cast<char *const *>(this->meta_vars_.data())
	);
	std::cerr << "webserv: [emerg] execve() failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
	std::exit(EXIT_FAILURE);
}

void	cgi::CGIExecutor::prepareCgiExecution(
	const HttpRequest& http_request,
	const std::string& script_path,
	const int socket
)
{
	if (redirectStdIOToSocket(socket))
		std::exit(EXIT_FAILURE); // responseをどうする？ bad gatewayでいいのか
	createScriptPath(script_path);
	createArgv(script_path);
	createMetaVars(http_request);
}

void	cgi::CGIExecutor::createScriptPath(const std::string& script_path)
{
	this->script_path_ = script_path;
	if (!FileUtils::isExtensionFile(script_path, ".php"))
		return;
	// スクリプトがphpの場合は、phpのpathを探す必要がある
	const std::string	path = searchCommandInPath("php");
	if (path == "")
		return;
	this->script_path_ = path;
}

void	cgi::CGIExecutor::createArgv(const std::string& script_path)
{
	const std::string::size_type	n = script_path.rfind("/");
	const std::string	cgi_script = n == std::string::npos ? script_path : script_path.substr(n + 1);
	this->argv_.push_back(cgi_script.c_str());
	this->argv_.push_back(NULL);
}

void	cgi::CGIExecutor::createMetaVars(const HttpRequest& http_request)
{
	(void)http_request;
	this->meta_vars_.push_back("AUTH_TYPE="); // Authorizationをparseするロジックを実装しないため、値は空文字

	// const std::string content_length = std::string("CONTENT_LENGTH=") + toStr(http_request.body.size());
	// this->meta_vars_.push_back(content_length.c_str());
	this->meta_vars_.push_back(NULL);
}

std::vector<std::string>	cgi::CGIExecutor::split(const std::string& s, char delimiter) const
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream is(s);
	while (std::getline(is, token, delimiter))
	{
		token = token == "" ? "." : token;
		tokens.push_back(token);
	}
	return tokens;
}

std::string cgi::CGIExecutor::searchCommandInPath(const std::string& command) const
{
	const char	*path = std::getenv("PATH");
	if (path == NULL)
		return "";
	std::vector<std::string> directories = split(path, ':');
	for (size_t i = 0; i < directories.size(); ++i)
	{
		std::string command_path = directories[i] + "/" + command;
		if (!FileUtils::isDirectory(command_path) && access(command_path.c_str(), X_OK) == 0)
			return command_path;
	}
	return "";
}

/**
 * @brief socketを標準入力と標準出力に複製する
 * 
 * 標準入力からはbodyを受け取る
 * 標準出力はcgiスクリプトのresponseを書き込む
 * 
 * @param socket 
 * @return true 
 * @return false 
 */
bool	cgi::CGIExecutor::redirectStdIOToSocket(const int socket) const
{
	if (dup2(socket, STDIN_FILENO) == -1)
	{
		std::cerr << "webserv: [emerg] dup2() failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		close(socket);
		return false;
	}

	if (dup2(socket, STDOUT_FILENO) == -1)
	{
		std::cerr << "webserv: [emerg] dup2() failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		close(STDIN_FILENO);
		close(socket);
		return false;
	}
	close(socket);
	return true;
}
