#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include <string>
# include <map>

class HttpResponse
{
	public:
		// initializer
		HttpResponse( const ConfigHandler& config_handler );

		// member methods
		static std::string	responseHandler( const HttpRequest& request, const struct TiedServer& tied_servers, const int client_sock );

	private:
		// private member availables
		std::string			status_code_;
		std::map<std::string, std::string>	headers_;
		std::string			body_;
		static std::map<int, std::string> webserv_status_line_map_; // status codeに基づくstatus lineを格納するmap
		static std::map<int, std::string> default_error_page_map_; // defaultのerror pageを格納するmap

		// private member objects
		ConfigHandler	config_handler_;

		// utils methods
		static void	
		static std::string	createStaticResponse( const std::string& body, const std::string& statusCode, const std::string& contentType );
		static std::string	autoIndex( const std::string& directoryPath );
};


#endif
