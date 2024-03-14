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
		std::string	cgi_path_;
		std::vector<const char*>	argv_;
		std::vector<const char*>	meta_vars_; // メタ変数(環境変数)
	public:
		CGIExecutor();
		~CGIExecutor();
		void	executeCgi(const HttpRequest& http_request);
		void	setCgiPath(const std::string& cgi_path);
		void	setMetaVars(const HttpRequest& http_request);
		void	setMessageBody();

};
} // namespace cgi

#endif
