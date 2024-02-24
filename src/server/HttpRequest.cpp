#include "HttpRequest.hpp"
#include <cctype>

HttpRequest::HttpRequest(const unsigned int method, const std::string& uri, const std::string& version,
			 const std::unordered_map<std::string, std::string>& headers,
			 const std::unordered_map<std::string, std::string>& queries,
			 const std::string& body,
			 const ParseState parseState)
			: method(method), uri(uri), version(version), headers(headers), queries(queries), body(body), parseState(parseState)
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest HttpRequest::parseRequest(std::string& rawRequest)
{
	enum parseRequestPhase {
		sw_start,
		sw_request_line,
		sw_headers,
		sw_body,
		sw_end
	} state;

	HttpRequest newRequest;
	state = sw_start;
	while (state != sw_end) {
		switch (state) {
		case sw_start:
			state = sw_request_line;
			break;
		case sw_request_line:
			newRequest.parseState = HttpRequest::parseRequestLine(rawRequest, newRequest);
			if ( newRequest.parseState == HttpRequest::PARSE_ERROR )
				return newRequest;
			state = sw_headers;
			break;
		case sw_headers:
			newRequest.parseState = HttpRequest::parseHeaders(rawRequest, newRequest);
			state = sw_body;
			break;
		case sw_body:
			// parseBody();
			state = sw_end;
			break;
		case sw_end:
			break;
		}
	}

	// std::unordered_unordered_map<int, int> aa;
	newRequest.parseState = HttpRequest::PARSE_COMPLETE;
	return newRequest;
}

void HttpRequest::parseChunked(HttpRequest& request)
{
	(void)request;
}

HttpRequest::ParseState HttpRequest::parseMethod(std::string& rawRequest, HttpRequest& newRequest)
{
	enum ParseMethodPhase {
		sw_method_start,
		sw_method_mid,
		sw_method_end,
	} state;

	std::string method;

	state = sw_method_start;
	for (size_t i = 0; i < rawRequest.size() && state != sw_method_end; ++i) {
		char ch = rawRequest[i];
		switch (state) {
		case sw_method_start:
			method += ch;
			state = sw_method_mid;
			break;
		case sw_method_mid:
			if (std::isalpha(ch)) {
				method += ch;
			} else {
				state = sw_method_end;
			}
			break;
		case sw_method_end:
			rawRequest = rawRequest.substr(i);
			break;
		}
	}

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

	return HttpRequest::PARSE_METHOD_DONE;
}

/* 
 * URLのパース
 * URIかと思っていた、、
 * URLからスキーマ、ポート、パス、クエリーに分解する？
 */
HttpRequest::ParseState HttpRequest::parseUri(std::string uri, HttpRequest& newRequest)
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
	return HttpRequest::PARSE_URI_DONE;
}

HttpRequest::ParseState HttpRequest::parseVersion(std::string version, HttpRequest& newRequest)
{
	if ( version != "HTTP/1.1" )
		return HttpRequest::PARSE_ERROR;
	newRequest.version = version;
	return HttpRequest::PARSE_VERSION_DONE;
}

HttpRequest::ParseState HttpRequest::parseRequestLine(std::string& rawRequest, HttpRequest& newRequest)
{
	enum parseRequestLineState {
		sw_start,
		sw_method,
		sw_uri,
		sw_version,
		sw_end
	} state;

	state = sw_start;
	for (size_t i = 0; i < rawRequest.size(); ++i) {
		switch (state) {
		case sw_start:
			state = sw_method;
			break;
		case sw_method:
			HttpRequest::parseMethod(rawRequest, newRequest);
			state = sw_uri;
			break;
		case sw_uri:
			HttpRequest::parseUri(rawRequest, newRequest);
			state = sw_version;
			break;
		case sw_version:
			if ( HttpRequest::parseVersion(rawRequest, newRequest) == false )
				return HttpRequest::PARSE_ERROR;
			state = sw_end;
			break;
		case sw_end:
			rawRequest = rawRequest.substr(i);
			break;
		default:
			break;
		};
	}
	return HttpRequest::PARSE_REQUEST_LINE_DONE;
}

/*
 * read each into hash??
 *
 *
 */
HttpRequest::ParseState HttpRequest::parseHeaders(const std::string& headers, HttpRequest& newRequest)
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

	std::string cur_name;
	std::string cur_value;
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
			if (!std::isalpha(ch)) {
				newRequest.headers[cur_name];
				state = sw_colon;
			}
			cur_name += ch;
			break;
		case sw_colon:
			if (ch != ':')
				return HttpRequest::PARSE_ERROR;
			state = sw_space_before_value;
			break;
		case sw_space_before_value:
			if (ch != ' ') {
				state = sw_value;
			}
			break;
		case sw_value:
			if (!std::isalpha(ch)) {
				newRequest.headers[cur_name] = cur_value;
				cur_name = "";
				cur_value = "";
				state = sw_space_after_value;
			}
			cur_value += ch;
			break;	
		case sw_space_after_value:
			if (ch != ' ') {
				state = sw_start;
			}
			break;
		default:
			// hmm
			break;
		}
	}
	return HttpRequest::PARSE_HEADER_DONE;
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

