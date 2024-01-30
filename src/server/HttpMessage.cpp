#include "HttpMessage.hpp"
#include "CGIHandler.hpp"

HttpRequest HttpMessage::requestParser( std::string &rawRequest )
{
	std::istringstream iss;
	HttpRequest requestline;
	std::string uriAndPath;

	iss.str( rawRequest );
	iss >> requestline.method >> uriAndPath >> requestline.version;

	requestline.uri = CGIHandler::getScriptPath(uriAndPath);
	requestline.query = CGIHandler::getQueryString(uriAndPath);

	return requestline;
}

bool httpUtils::isDirectory(const std::string& path)
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

std::string httpUtils::createResponse(const std::string& body, const std::string& statusCode = "200 OK", const std::string& contentType = "text/html")
{
	std::stringstream response;
	response << "HTTP/1.1 " << statusCode << "\r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << body.length() << "\r\n";
	response << "\r\n";
	response << body;
	return response.str();
}

std::string httpUtils::readFile(const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string httpUtils::listDirectory(const std::string& directoryPath)
{
	std::stringstream buffer;
	buffer << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Directory listing for</title></head>";
	buffer << "<body><h1>Directory listing for " << directoryPath << "</h1>";
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

	if ( httpUtils::isDirectory(request.uri) )
	{
		std::string indexPath = request.uri + "/index.html";
		std::ifstream ifile( indexPath.c_str() );
		if ( ifile )
		{
			return httpUtils::createResponse( httpUtils::readFile(indexPath) );
		}
		else
		{
			return httpUtils::createResponse( httpUtils::listDirectory(request.uri) );
		}
	}
	else
	{
		std::ifstream ifile( request.uri.c_str() );
		if ( ifile )
		{
			if ( CGIHandler::isCGI( request.uri ) )
				return httpUtils::createResponse( CGIHandler::executeCGI( request.uri, request.query ) );
			return httpUtils::createResponse( httpUtils::readFile(request.uri) );
		}
		else
		{
			return httpUtils::createResponse( httpUtils::readFile("html/404.html"), "404 Not Found" );
		}
	}
}

