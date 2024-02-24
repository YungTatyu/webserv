#include "HttpRequest.hpp"
#include <functional>

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
	newRequest.parseState = HttpRequest::parseHeaders(iss, newRequest);
	newRequest.parseState = HttpRequest::PARSE_COMPLETE;
	return newRequest;
}

void HttpRequest::parseChunked(HttpRequest& request)
{
	(void)request;
}

/* 
 * URLのパース
 * URIかと思っていた、、
 * URLからスキーマ、ポート、パス、クエリーに分解する？
 */
void HttpRequest::parseUri(std::string uri, HttpRequest& newRequest)
{
	enum parseUriPhase {
		sw_start,
		sw_uriBeforeSlash,
		sw_schema,
		sw_end
	};
	parseUriPhase state = sw_start;
	// ここでステートマシンを実装する
	for (size_t i = 0; i < uri.size(); ++i) {
		switch (state) {
		case sw_start:
			state = sw_start;
		case sw_uriBeforeSlash:
			state = sw_schema;
		case sw_schema:
			state = sw_end;
		case sw_end:
			newRequest.uri = uri;	
			break;
		}
	}
}

bool HttpRequest::parseVersion(std::string version, HttpRequest& newRequest)
{
	if ( version != "HTTP/1.1" )
		return false;
	newRequest.version = version;
	return true;
}

HttpRequest::ParseState HttpRequest::parseRequestLine(std::istringstream& requestLine, HttpRequest& newRequest)
{
	std::string method;
	std::string uri;
	std::string version;
	requestLine >> method >> uri >> version;
	switch (method.size()) {
	case 3:
		if (method == "GET")
		{
			newRequest.method = GET;
			break;
		}
		return HttpRequest::PARSE_ERROR;
	case 4:
		if (method == "HEAD")
		{
			newRequest.method = HEAD;
			break;

		}
		else if (method == "POST")
		{
			newRequest.method = POST;
			break;
		}
		return HttpRequest::PARSE_ERROR;
	default:
		return HttpRequest::PARSE_ERROR; // 501 Not Implemented (SHOULD)
		break;
	}

	HttpRequest::parseUri(uri, newRequest);

	if ( HttpRequest::parseVersion(version, newRequest) == false )
		return HttpRequest::PARSE_ERROR;

	return HttpRequest::PARSE_INPROGRESS;
}

/*
 * read each into hash??
 *
 *
 */
HttpRequest::ParseState HttpRequest::parseHeaders(std::string& headers, HttpRequest& newRequest)
{
	enum parseHeaderPhase {
		sw_start,
		sw_name,
		sw_colon,
		sw_space_before_value,
		sw_value,
		sw_space_after_value,
		sw_almost_done,
		sw_header_almost_done,
		sw_end
	} state;

	state = sw_start;
	for (size_t i = 0; i < headers.size(); ++i) {
		char ch = headers[i];
		switch (state) {
		case sw_start:
			if (ch == '\r')
				state = sw_header_almost_done;
			else if (ch == '\n') {
				state = sw_end;
				return HttpRequest::PARSE_INPROGRESS;
			}
			else {
				return HttpRequest::PARSE_ERROR;
			}
			state = sw_name;
			break;
		case sw_name:
			state = sw_colon;
			break;
		case sw_colon:
			if (ch != ':')
				return HttpRequest::PARSE_ERROR;
			state = sw_space_before_value;
			break;
		case sw_space_before_value:
			if (ch != ' ')
				return HttpRequest::PARSE_ERROR;
			state = sw_value;
			break;
		case sw_value:
			//store value here;
			state = sw_space_after_value;
			break;	
		case sw_space_after_value:
			state = sw_start;
			break;
		default:
			// hmm
			break;
		}
	}

	(void)newRequest;
	return HttpRequest::PARSE_INPROGRESS;
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

