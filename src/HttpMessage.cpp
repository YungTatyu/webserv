#include "HttpMessage.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

HttpRequest HttpMessage::requestParser( std::string &rawRequest )
{
	std::istringstream iss;
	HttpRequest requestline;

	iss.str( rawRequest );
	iss >> requestline.method >> requestline.uri >> requestline.version;

	std::cout << "method=" << "\"" << requestline.method << "\"" << std::endl;
	std::cout << "uri=" << "\"" << requestline.uri << "\"" << std::endl;
	std::cout << "version=" << "\"" << requestline.version << "\"" << std::endl;

	return requestline;
}

std::string HttpMessage::responseGenerater( HttpRequest &request )
{	
	std::ifstream file( request.uri );
	std::stringstream buffer;
	std::string responseBody;

	if (file.is_open())
	{
		buffer << file.rdbuf();
		file.close();
		responseBody = buffer.str();  
	}
	else
	{
		responseBody = "<html><body><h1>File not found.</h1></body></html>\n";
	}	
	
    std::string response;

    response += "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html; charset=UTF-8\r\n";
    response += "Content-Length: " + std::to_string(responseBody.length()) + "\r\n";
    response += "\r\n";
    response += responseBody;

    return response;
}

