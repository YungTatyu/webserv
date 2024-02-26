#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include <string>
# include <map>

class HttpResponse
{
	public:
		// initializer
		HttpResponse( const ConfigHandler& config_handler );

		// member availables
		static std::string http_version_ = "HTTP/1.1";
		std::string status_code_;
		std::stiring reason_phrase_;
		std::map<std::string, std::string> headers_;
		std::string body_;

		// member methods
		static std::string responseHandler( const HttpRequest& request, const struct TiedServer& tied_servers, const int client_sock );

	private:
		// member objects
		ConfigHandler	config_handler_;

		// utils methods
		static std::string createStaticResponse( const std::string& body, const std::string& statusCode, const std::string& contentType );
		static std::string autoIndex( const std::string& directoryPath );
};


#endif
