#ifndef CGI_CGI_HANDLER_HPP
#define CGI_CGI_HANDLER_HPP

#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "CGIParser.hpp"
#include "CGIExecutor.hpp"

namespace cgi
{
enum CGI_SOCKET
{
	SOCKET_PARENT = 0,
	SOCKET_CHILD = 1,
	SOCKET_READ = 0,
	SOCKET_WRITE = 1,
};

class CGIHandler
{
	private:
		CGIParser	cgi_parser_;
		CGIExecutor	cgi_executor_;
		pid_t	cgi_process_id_;
		bool	forkCgiProcess(
			const HttpRequest& request,
			const std::string& script_path
		);
	public:
		int	sockets_[2];
		CGIHandler();
		~CGIHandler();
		static bool	isCgi(const std::string& script_path);
		bool	callCgiExecutor(
			const std::string& script_path,
			const HttpRequest& request
		);
		void	callCgiParser(HttpResponse& response);
		void	killCgiProcess() const;
		const CGIParser&	getCgiParser() const;
		const CGIExecutor&	getCgiExecutor() const;
		const pid_t&	getCgiProcessId() const;
		void	resetSockets();
};
} // namespace cgi

#endif

