#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sys/stat.h>

class HttpRequest
{
	public:
		static std::string setQueryString( std::string& uri );
		static std::string setScriptPath( std::string& uri );

		std::string method;
		std::string uri;
		std::string version;
		std::string query;
	// header, body
};

//struct HttpResponse
//{
//	std::string status;
//	std::string body;
//	// header
//};

class HttpMessage
{
	public:
		static HttpRequest requestParser( std::string &rawRequest );
		static std::string responseGenerater( HttpRequest &request );

		static std::string autoIndex(const std::string& directoryPath);
		static std::string createResponse(const std::string& body, const std::string& statusCode, const std::string& contentType);

	private:
		HttpMessage();
};

#endif
