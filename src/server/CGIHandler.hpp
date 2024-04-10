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
		int	cli_socket_; // cgiが紐づくクライアント
		pid_t	cgi_process_id_;
		int	cli_socket_; // cgiが紐づくクライアント
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
		bool	callCgiParser(HttpResponse& response, const std::string& cgi_response);
		void	killCgiProcess() const;
		const CGIParser&	getCgiParser() const;
		const CGIExecutor&	getCgiExecutor() const;
		pid_t	getCgiProcessId() const;
		int	getCliSocket() const;
		void	setCliSocket(const int socket);
		int	getCgiSocket() const;
		void	resetSockets();
};
} // namespace cgi

#endif

