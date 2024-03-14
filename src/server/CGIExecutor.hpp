#ifndef CGI_CGI_EXECUTOR_HPP
#define CGI_CGI_EXECUTOR_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include <unistd.h>

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
		pid_t	cgi_process_id_;
	public:
		CGIExecutor();
		~CGIExecutor();
		void	executeCgi(const std::string& cgi_path, const HttpRequest& http_request);
		void	setCgiPath(const std::string& cgi_path);
		void	setMetaVars(const HttpRequest& http_request);
		pid_t	getCgiProcessId() const;
};
} // namespace cgi

#endif
