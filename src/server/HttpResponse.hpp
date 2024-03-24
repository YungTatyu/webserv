#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <map>

# include "ConfigHandler.hpp"
# include "HttpRequest.hpp"
# include "NetworkIOHandler.hpp"
# include "Utils.hpp"

class HttpResponse
{
	public:
		// initializer
		HttpResponse();

		// member methods
		static std::string	generateResponse( HttpRequest& request, HttpResponse& response, const struct TiedServer& tied_servers, const int client_sock, const ConfigHandler& config_handler );
		static std::string	createResponse( const HttpResponse& response );

		// public variables
		std::string	root_path_;
		std::string	cgi_status_code_line_;
		long	status_code_; // response生成するときにstatus_line_map_参照する
		std::map<std::string, std::string, Utils::CaseInsensitiveCompare>	headers_;
		std::string			body_;
		static std::map<int, std::string> status_line_map_; // status codeに基づくstatus lineを格納するmap
		static std::map<int, const std::string*> default_error_page_map_; // defaultのerror pageを格納するmap

	private:
				// private member
		size_t	internal_redirect_cnt_;
		static const size_t kMaxInternalRedirect = 10;

		// utils methods
		// 名前微妙
		static std::string	autoIndex( const std::string& directory_path );
		static std::string	getCurrentGMTTime();
		static int	returnPhase( HttpResponse& response, const config::Location* location );
		static void	returnResponse( HttpResponse& response, const config::Return& return_directive );
		static int	contentHandler( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location, const ConfigHandler& config_handler );
		static int	staticHandler( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location, const ConfigHandler& config_handler );
		static int	Index( HttpResponse& response, HttpRequest& request, const std::vector<config::Index>& index_list, bool is_autoindex_on );
		static int	TryFiles( HttpResponse& response, HttpRequest& request, const config::TryFiles& try_files );
		static int	errorPagePhase( HttpResponse& response, HttpRequest& request, const config::Server& server, const config::Location* location, const ConfigHandler& config_handler );
		bool	isURL( const std::string uri ) const;
		static void	headerFilterPhase( HttpResponse& response );
		static std::string	detectContentTypeFromBody( const std::string& body );
		static std::string	transformLetter( const std::string& key_str );
};


#endif
