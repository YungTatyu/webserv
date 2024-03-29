#ifndef CGI_CGI_EXECUTOR_HPP
#define CGI_CGI_EXECUTOR_HPP

#include <string>
#include <vector>

#include "HttpRequest.hpp"

namespace cgi
{
class CGIExecutor
{
	private:
		std::string	cgi_path_;
		std::vector<const char*>	argv_;
		std::vector<const char*>	meta_vars_; // メタ変数(環境変数)
		void	setMessageBody(const std::string& body) const;
		std::vector<std::string>	split(const std::string& s, char delimiter) const;
		std::string	searchCommandPath(const std::string& command) const;
	public:
		CGIExecutor();
		~CGIExecutor();
		void	executeCgi(const HttpRequest& http_request);
		const std::string&	getCgiPath() const;
		void	setCgiPath(const std::string& cgi_path);
		const std::vector<const char*>&	getArgv() const;
		void	setArgv(const std::string& argv);
		const std::vector<const char*>&	getMetaVars() const;
		void	setMetaVars(const HttpRequest& http_request);
};
} // namespace cgi

#endif
