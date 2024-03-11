#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include <string>
# include <map>

# include "ConfigHandler.hpp"
# include "HttpRequest.hpp"
# include "NetworkIOHandler.hpp"

class HttpResponse
{
	public:
		// initializer
		HttpResponse( const ConfigHandler& config_handler );

		// member methods
		static std::string	generateResponse( HttpRequest& request, const struct TiedServer& tied_servers, const int client_sock, const ConfigHandler& config_handler );
		static std::string	createResponse( const HttpResponse& response );

	private:
		// private variables
		unsigned int		status_code_; // response生成するときにstatus_line_map_参照する
		std::map<std::string, std::string>	headers_;
		std::string			body_;
		static std::map<int, std::string> status_line_map_; // status codeに基づくstatus lineを格納するmap
		static std::map<int, const std::string*> default_error_page_map_; // defaultのerror pageを格納するmap

		// private member objects
		const ConfigHandler	&config_handler_;

		// utils methods
		// 名前微妙
		static std::string	autoIndex( const std::string& directoryPath );
		static std::string	getCurrentGMTTime();
		static bool	errorPagePhase( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location);
		static bool	returnPhase( HttpResponse& response, const config::Location* location );
		static void	returnResponse( HttpResponse& response, const config::Return& return_directive );
		//bool	contentHandler( HttpResponse& response, HttpRequest& request, config::Server& server, config::Location* location );
		//void	prepareResponse( const HttpRequest& request, const config::Server& server, struct sockaddr_in client_addr );
		//void	prepareErrorResponse( const HttpRequest& request, const config::Server& server, const config::Location* location, const struct sockaddr_in client_addr, const unsigned int code );
		//void	internalRedirect( const HttpRequest& request, const config::Server& server, const struct sockaddr_in& client_addr, std::string redirect_uri );
};


#endif
