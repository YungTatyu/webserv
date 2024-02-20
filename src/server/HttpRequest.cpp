#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const unsigned int method, const std::string& uri, const std::string& version,
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
	HttpRequest newRequest;
	std::istringstream iss(rawRequest);
	HttpRequest::parseRequestLine(iss, newRequest);
	newRequest.parseState = HttpRequest::PARSE_COMPLETE;
	return newRequest;
}

void HttpRequest::parseChunked(HttpRequest& request)
{
	(void)request;
}

void HttpRequest::parseUri()
{
}

void HttpRequest::parseRequestLine(std::istringstream& requestLine, HttpRequest& newRequest)
{
	requestLine >> newRequest.method >> newRequest.uri >> newRequest.version;
}

void HttpRequest::parseHeaders(std::istringstream& headers, HttpRequest& newRequest)
{
	(void)headers;
	(void)newRequest;
}

void HttpRequest::parseBody(std::istringstream& body, HttpRequest& newRequest)
{
	(void)body;
	(void)newRequest;
}

std::string HttpRequest::urlDecode(const std::string& str, HttpRequest& newRequest)
{
	(void)str;
	(void)newRequest;
	return "";
}

