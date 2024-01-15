#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

#include <string>

namespace httpUtils
{
	bool isDirectory(const std::string& path);
	std::string createResponse(const std::string& body, const std::string& statusCode, const std::string& contentType);
	std::string readFile(const std::string& filePath);
	std::string listDirectory(const std::string& directoryPath);
}

struct HttpRequest
{
	std::string method;
	std::string uri;
	std::string version;
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
		// HttpResponse static responseGenerater( HttpRequest requeset );
		HttpRequest static requestParser( std::string &rawRequest );
		std::string static responseGenerater( HttpRequest &request );

	private:
		HttpMessage();
};

#endif
