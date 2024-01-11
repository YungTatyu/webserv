#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

#include <string>

class HttpRequest
{
	public:
		std::string method;
		std::string uri;
		std::string version;
		// header, body
};

class HttpResponse
{
	public:
		std::string status;
		std::string body;
		// header
};

class HttpMessage
{
	public:
		// HttpRequest static requestParser( std::string rawRequest );
		// HttpResponse static responseGenerater( HttpRequest requeset );
		// HttpRequest static requestParser( std::string rawRequest );
		std::string static responseGenerater( std::string request );
};

#endif
