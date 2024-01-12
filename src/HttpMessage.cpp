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

#include <sys/types.h>
#include <dirent.h>
std::string createHtmlLink(const std::string& path, bool isDir)
{
	std::string absolutePath = "./" + path;
	std::cout << absolutePath << std::endl;
	std::string link = "<a href=\"" + path + "\">" + path + "</a>";
	if (isDir)
	{
		link += "/";
	}
	return link;
}

#include <sys/stat.h>
bool isDirectory(const std::string& path)
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

std::string HttpMessage::responseGenerater( HttpRequest &request )
{	
	// if (request.uri == "/") // デフォルトアクセスは/
	// 	request.uri = "/index.html";
	request.uri = std::string(".") + request.uri;
	std::ifstream file( request.uri );
	std::stringstream buffer;
	std::string responseBody;
	DIR *dir;
	struct dirent *ent;

	if (file.is_open())
	{
		if ( isDirectory( request.uri ) )
		{
			const char *directoryPath = request.uri.c_str();	
			if ((dir = opendir(directoryPath)) != NULL)
			{
				responseBody = "<html><body><h1>Directory listing for / </h1>";
				responseBody += "<hr>";
				responseBody += "<ul>\n";
				while ((ent = readdir(dir)) != NULL)
				{
					std::string path = ent->d_name;
					bool isDir = (ent->d_type == DT_DIR);
					responseBody += "<li>";
					responseBody += createHtmlLink(path, isDir);
					responseBody += "</li>\n";
				}
				responseBody += "</ul>\n";
				responseBody += "<hr>\n";
				responseBody += "</body></html>\n";
				closedir(dir);
			}
			else
			{
				std::cerr << "Error opening directory." << std::endl;
				responseBody = "Error opening directory.";
			}	
		}
		else
		{
			buffer << file.rdbuf();
			responseBody = buffer.str();  
		}
		file.close();
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

