#include "HttpMessage.hpp"
#include "CGIHandler.hpp"
#include "FileUtils.hpp"
//
// std::string HttpRequest::setQueryString( std::string& uri )
// {
// 	std::size_t pos = uri.find("?");
// 	if ( pos != std::string::npos )
// 		return uri.substr( pos + 1 );
// 	return "";
// }
//
// std::string HttpRequest::setScriptPath( std::string& uri )
// {
// 	return uri.substr(0, uri.find("?"));
// }
//
// HttpRequest HttpMessage::requestParser( std::string &rawRequest )
// {
// 	std::istringstream iss;
// 	HttpRequest requestline;
// 	std::string uriAndPath;
//
// 	iss.str( rawRequest );
// 	iss >> requestline.method >> uriAndPath >> requestline.version;
//
// 	requestline.uri = HttpRequest::setScriptPath(uriAndPath);
// 	requestline.query = HttpRequest::setQueryString(uriAndPath);
//
// 	return requestline;
// }

std::string HttpMessage::autoIndex(const std::string& directoryPath)
{
	std::vector<std::string> contents = FileUtils::getDirectoryContents(directoryPath);
	std::stringstream buffer;
	buffer << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Directory listing for</title></head>";
	buffer << "<body><h1>Directory listing for " << directoryPath << "</h1>";
	buffer << "<hr>";
	buffer << "<ul>";

	for (std::vector<std::string>::iterator it = contents.begin(); it != contents.end(); ++it)
	{
		buffer << "<li><a href='" << directoryPath;
		if (!directoryPath.empty() && directoryPath[directoryPath.size() - 1] != '/')
		    buffer << "/";
		buffer << *it << "'>" << *it << "</a></li>";
	}

	buffer << "</ul>";
	buffer << "<hr>";
	buffer << "</body></html>";
	return buffer.str();
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

std::string HttpMessage::responseGenerater( HttpRequest &request )
{	
	request.uri = std::string(".") + request.uri;

	if ( FileUtils::isDirectory(request.uri) )
	{
		std::string indexPath = request.uri + "/index.html";
		std::ifstream ifile( indexPath.c_str() );
		if ( ifile )
		{
			return HttpMessage::createResponse( FileUtils::readFile(indexPath) );
		}
		else
		{
			return HttpMessage::createResponse( HttpMessage::autoIndex(request.uri) );
		}
	}
	else
	{
		std::ifstream ifile( request.uri.c_str() );
		if ( ifile )
		{
			if ( CGIHandler::isCGI( request.uri ) )
				return HttpMessage::createResponse( CGIHandler::executeCGI( request.uri, request.queries ) );
			return HttpMessage::createResponse( FileUtils::readFile(request.uri) );
		}
		else
		{
			return HttpMessage::createResponse( FileUtils::readFile("html/404.html"), "404 Not Found" );
		}
	}
}

