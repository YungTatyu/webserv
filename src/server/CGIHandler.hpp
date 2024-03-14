#ifndef CGI_CGI_HANDLER_HPP
#define CGI_CGI_HANDLER_HPP

#include <string>

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
	public:
		CGIParser	cgi_parser_;
		CGIExecutor	cgi_executor_;
		int	sockets_[2];
		CGIHandler();
		~CGIHandler();
		static bool	isCgi(const std::string& cgi_path);
		void	setMetaVariables();
		void	setMessageBody();
		void	createCgiProcess();
		void	callCgiExecutor();
		void	callCgiParser();
		void	killCgiProcess();
};
} // namespace cgi

#endif

