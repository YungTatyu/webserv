#include "HttpMessage.hpp"
#include "CGIHandler.hpp"

std::string HttpMessage::getQueryString( std::string& uri )
{
	std::size_t pos = uri.find("?");
	if ( pos != std::string::npos )
		return uri.substr( pos + 1 );
	return "";
}

std::string HttpMessage::getScriptPath( std::string& uri )
{
	return uri.substr(0, uri.find("?"));
}

HttpRequest HttpMessage::requestParser( std::string &rawRequest )
{
	std::istringstream iss;
	HttpRequest requestline;
	std::string uriAndPath;

	iss.str( rawRequest );
	iss >> requestline.method >> uriAndPath >> requestline.version;

	requestline.uri = HttpMessage::getScriptPath(uriAndPath);
	requestline.query = HttpMessage::getQueryString(uriAndPath);

	return requestline;
}

bool HttpMessage::isDirectory(const std::string& path)
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

std::string HttpMessage::createResponse(const std::string& body, const std::string& statusCode = "200 OK", const std::string& contentType = "text/html")
{
	std::stringstream response;
	response << "HTTP/1.1 " << statusCode << "\r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << body.length() << "\r\n";
	response << "\r\n";
	response << body;
	return response.str();
}

std::string HttpMessage::readFile(const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string HttpMessage::listDirectory(const std::string& directoryPath)
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

	if ( HttpMessage::isDirectory(request.uri) )
	{
		std::string indexPath = request.uri + "/index.html";
		std::ifstream ifile( indexPath.c_str() );
		if ( ifile )
		{
			return HttpMessage::createResponse( HttpMessage::readFile(indexPath) );
		}
		else
		{
			return HttpMessage::createResponse( HttpMessage::listDirectory(request.uri) );
		}
	}
	else
	{
		std::ifstream ifile( request.uri.c_str() );
		if ( ifile )
		{
			if ( CGIHandler::isCGI( request.uri ) )
				return HttpMessage::createResponse( CGIHandler::executeCGI( request.uri, request.query ) );
			return HttpMessage::createResponse( HttpMessage::readFile(request.uri) );
		}
		else
		{
			return HttpMessage::createResponse( HttpMessage::readFile("html/404.html"), "404 Not Found" );
		}
	}
}

