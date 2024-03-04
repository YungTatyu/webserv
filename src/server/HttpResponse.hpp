#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include <string>
# include <map>

# include "ConfigHandler.hpp"
//# include "HttpRequst.hpp"
# include "NetworkIOHandler.hpp"

class HttpResponse
{
	public:
		// initializer
		HttpResponse( const ConfigHandler& config_handler );

		// member methods
		void	prepareResponse( const HttpRequest& request, const struct TiedServer& tied_servers, const int client_sock );
		static std::string	createStaticResponse();

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
		void	responseHandler( const HttpRequest& request, const config::Server& server, struct sockaddr_in client_addr );
		static std::string	autoIndex( const std::string& directoryPath );
		void	prepareErrorResponse( const HttpRequest& request, const config::Server& server, const config::Location* location, const struct sockaddr_in client_addr, const unsigned int code );
		void	internalRedirect( const HttpRequest& request, const config::Server& server, const struct sockaddr_in& client_addr, std::string redirect_uri );
};


#endif
