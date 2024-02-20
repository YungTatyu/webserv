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
	newRequest.parseState = HttpRequest::parseRequestLine(iss, newRequest);
	if ( newRequest.parseState == HttpRequest::PARSE_ERROR )
		return newRequest;
	newRequest.parseState = HttpRequest::PARSE_COMPLETE;
	return newRequest;
}

void HttpRequest::parseChunked(HttpRequest& request)
{
	(void)request;
}

void HttpRequest::parseUri(std::string uri, HttpRequest& newRequest)
{
	newRequest.uri = uri;
}

void HttpRequest::parseVersion(std::string version, HttpRequest& newRequest)
{
	newRequest.version = version;
}

HttpRequest::ParseState HttpRequest::parseRequestLine(std::istringstream& requestLine, HttpRequest& newRequest)
{
	std::string method;
	std::string uri;
	std::string version;
	requestLine >> method >> uri >> version;
	switch (method.size()){
	case 3:
		if (method == "GET")
			newRequest.method = GET;
		break;
	case 4:
		if (method == "HEAD")
			newRequest.method = HEAD;
		else if (method == "POST")
			newRequest.method = POST;
		break;
	default:
		return HttpRequest::PARSE_ERROR; // 501 Not Implemented (SHOULD)
		break;
	}

	HttpRequest::parseUri(uri, newRequest);
	HttpRequest::parseVersion(version, newRequest);
	return HttpRequest::PARSE_INPROGRESS;
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

