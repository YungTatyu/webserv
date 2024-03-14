#ifndef CGI_CGI_EXECUTOR_HPP
#define CGI_CGI_EXECUTOR_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include <unistd.h>

namespace cgi
{
class CGIExecutor
{
	private:
		std::string	path_;
		std::vector<const char*>	argv_;
		std::vector<const char*>	envp_;
		pid_t	cgi_process_id_;
	public:
		CGIExecutor();
		~CGIExecutor();
};
} // namespace cgi

#endif
