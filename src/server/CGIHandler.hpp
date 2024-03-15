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
	SOCKET_CHILD = 1
};

class CGIHandler
{
	private:
		CGIParser	cgi_parser_;
		CGIExecutor	cgi_executor_;
		pid_t	cgi_process_id_;
		void	forkCgiProcess();
	public:
		int	sockets_[2];
		CGIHandler();
		~CGIHandler();
		static bool	isCgi(const std::string& cgi_path);
		void	callCgiExecutor(const std::string& cgi_path, const HttpRequest& http_request, const int socket);
		void	callCgiParser(HttpResponse& http_response);
		void	killCgiProcess() const;
		const CGIParser&	getCgiParser() const;
		const CGIExecutor&	getCgiExecutor() const;
};
} // namespace cgi

#endif

