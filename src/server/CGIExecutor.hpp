#ifndef CGI_CGI_EXECUTOR_HPP
#define CGI_CGI_EXECUTOR_HPP

#include <string>
#include <vector>

// #include "HttpRequest.hpp"
#include "HttpMessage.hpp"

namespace cgi
{
class CGIExecutor
{
	private:
		std::string	script_path_;
		std::vector<const char*>	argv_;
		std::vector<const char*>	meta_vars_; // メタ変数(環境変数)
		void	prepareCGIExecution(const HttpRequest& http_request, const std::string& script_path, const int socket);
		void	createScriptPath(const std::string& script_path);
		void	createArgv(const std::string& script_path);
		void	createMetaVars(const HttpRequest& http_request);
		void	setMessageBody(const std::string& body) const;
		std::vector<std::string>	split(const std::string& s, char delimiter) const;
		std::string	searchCommandPath(const std::string& command) const;
	public:
		CGIExecutor();
		~CGIExecutor();
		void	executeCgiScript(const HttpRequest& http_request, const std::string& script_path, const int socket);
		const std::string&	getCgiPath() const;
		const std::vector<const char*>&	getArgv() const;
		const std::vector<const char*>&	getMetaVars() const;
};
} // namespace cgi

#endif
