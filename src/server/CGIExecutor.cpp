#include "CGIExecutor.hpp"
#include "FileUtils.hpp"

#include <unistd.h>
#include <cstdlib>

cgi::CGIExecutor::CGIExecutor() {}

cgi::CGIExecutor::~CGIExecutor() {}

void	cgi::CGIExecutor::executeCgiScript(
	const HttpRequest& http_request,
	const std::string& script_path,
	const int socket
)
{
	(void)http_request;
	(void)script_path;
	(void)socket;
}

void	cgi::CGIExecutor::prepareCGIExecution(
	const HttpRequest& http_request,
	const std::string& script_path,
	const int socket
)
{
	(void)socket;

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
	const std::string	path = searchCommandPath("php");
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


std::vector<std::string>	cgi::CGIExecutor::split(const std::string& s, char delimiter) const
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		token = token == "" ? "." : token;
		tokens.push_back(token);
	}
	return tokens;
}

std::string cgi::CGIExecutor::searchCommandPath(const std::string& command) const
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
