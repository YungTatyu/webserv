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

	// std::cout << system("cat index.html") << std::endl;

	std::ifstream file("index.html");
	std::stringstream buffer;

	if (file.is_open())
	{
		buffer << file.rdbuf();
		file.close();
		std::string fileContent = buffer.str();  
	}
	else
	{
		std::cout << "could not open file" << std::endl;
	}

	return requestline;
}

std::string HttpMessage::responseGenerater( std::string request )
{
    (void)request;
    std::string response;

    std::string responseBody = "<html><body><h1>Hello, World!</h1></body></html>\n";

    // ステータスライン
    response += "HTTP/1.1 200 OK\r\n";

    // ヘッダー
    response += "Content-Type: text/html; charset=UTF-8\r\n";
    response += "Content-Length: " + std::to_string(responseBody.length()) + "\r\n";
    response += "\r\n";

    // ボディ
    response += responseBody;

    return response;
}

