#include "HttpMessage.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

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

bool isDirectory(const std::string& path)
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

std::string createResponse(const std::string& body, const std::string& statusCode = "200 OK", const std::string& contentType = "text/html")
{
	std::stringstream response;
	response << "HTTP/1.1 " << statusCode << "\r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << body.length() << "\r\n";
	response << "\r\n";
	response << body;
	return response.str();
}

std::string readFile(const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string listDirectory(const std::string& directoryPath)
{
	std::stringstream buffer;
	buffer << "<html><body><h1>Directory listing for " << directoryPath << "</h1>";
	buffer << "<hr>";
	buffer << "<ul>";

	DIR* dir = opendir(directoryPath.c_str());
	if (dir != NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string filename = entry->d_name;
			buffer << "<li><a href='" << directoryPath;
			if ( !directoryPath.empty() && directoryPath[directoryPath.size() - 1] != '/' )
				buffer << "/";
			buffer << filename << "'>" << filename << "</a>";
			if ( isDirectory(directoryPath + "/" + filename) )
				buffer << "/";
			buffer << "</li>";
		}
		closedir(dir);
	}

	buffer << "</ul>";
	buffer << "<hr>";
	buffer << "</body></html>";
	return buffer.str();
}

std::string HttpMessage::responseGenerater( HttpRequest &request )
{	
	request.uri = std::string(".") + request.uri;

	if ( isDirectory(request.uri) )
	{
		std::string indexPath = request.uri + "/index.html";
		std::ifstream ifile( indexPath.c_str() );
		if ( ifile )
		{
			return createResponse( readFile(indexPath) );
		}
		else
		{
			return createResponse( listDirectory(request.uri) );
		}
	}
	else
	{
		std::ifstream ifile( request.uri.c_str() );
		if ( ifile )
		{
			return createResponse( readFile(request.uri) );
		}
		else
		{
			return createResponse( readFile("data/404.html"), "404 Not Found" );
		}
	}
}

