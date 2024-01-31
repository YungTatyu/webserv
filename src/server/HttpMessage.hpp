#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <sys/stat.h>

struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string version;
	std::string query;
	// header, body
};

struct HttpResponse
{
	std::string status;
	std::string body;
	// header
};

class HttpMessage
{
	public:
		static std::string getQueryString( std::string& uri );
		static std::string getScriptPath( std::string& uri );
		static HttpRequest requestParser( std::string &rawRequest );
		static std::string responseGenerater( HttpRequest &request );

		static bool isDirectory(const std::string& path);
		static std::string createResponse(const std::string& body, const std::string& statusCode, const std::string& contentType);
		static std::string readFile(const std::string& filePath);
		static std::string listDirectory(const std::string& directoryPath);

	private:
		HttpMessage();
};

#endif
