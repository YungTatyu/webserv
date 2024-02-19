#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const std::string& method, const std::string& uri, const std::string& version,
			 const std::map<std::string, std::string>& headers,
			 const std::map<std::string, std::string>& queries,
			 const std::string& body,
			 const ParseState parseState)
			: method(method), uri(uri), version(version), headers(headers), queries(queries), body(body), parseState(parseState)
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest HttpRequest::parseRequest(const std::string& rawRequest)
{
	(void)rawRequest;
	return HttpRequest();
}

void HttpRequest::parseChunked(HttpRequest& request)
{
	(void)request;
}

void HttpRequest::parseUri()
{
}

void HttpRequest::parseRequestLine(std::istringstream& requestLine)
{
	(void)requestLine;
}

void HttpRequest::parseHeaders(std::istringstream& headers)
{
	(void)headers;
}

void HttpRequest::parseBody(std::istringstream& body)
{
	(void)body;
}

std::string HttpRequest::urlDecode(const std::string& str)
{
	(void)str;
	return "";
}


