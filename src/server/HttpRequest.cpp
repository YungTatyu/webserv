#include "HttpRequest.hpp"
#include <cctype>

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
			HttpRequest::parseBody(rawRequest, newRequest);
			state = sw_end;
			break;
		case sw_end:
			break;
		}
	}

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
		sw_method_almost_end,
		sw_method_end,
	} state;

	std::string method;

	state = sw_method_start;
	size_t i = 0;
	while (state != sw_method_end && i < rawRequest.size()) {
		char ch = rawRequest[i];
		switch (state) {
		case sw_method_start:
			method += ch;
			state = sw_method_mid;
			break;
		case sw_method_mid:
			if (std::isalpha(static_cast<unsigned char>(ch))) {
				method += ch;
			} else {
				state = sw_method_almost_end;
			}
			break;
		case sw_method_almost_end:
			rawRequest = rawRequest.substr(i);
			state = sw_method_end;
			break;
		case sw_method_end:
			break;
		}
		++i;
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
HttpRequest::ParseState HttpRequest::parseUri(std::string& rawRequest, HttpRequest& newRequest)
{
	enum parseUriPhase {
		sw_start,
		sw_uriBeforeSlash,
		sw_schema,
		sw_almost_end,
		sw_end
	} state;

	state = sw_start;
	// ここでステートマシンを実装する
	while (state != sw_end) {
		switch (state) {
		case sw_start:
			state = sw_start;
		case sw_uriBeforeSlash:
			state = sw_schema;
		case sw_schema:
			state = sw_almost_end;
		case sw_almost_end:
			state = sw_end;
		case sw_end:
			break;
		}
	}
	newRequest.uri = rawRequest.substr(0, rawRequest.find(' '));
	rawRequest = rawRequest.substr(rawRequest.find(' ') + 1);
	return HttpRequest::PARSE_URI_DONE;
}

HttpRequest::ParseState HttpRequest::parseVersion(std::string& rawRequest, HttpRequest& newRequest)
{
	// if ( version != "HTTP/1.1" )
	// 	return HttpRequest::PARSE_ERROR;
	// newRequest.version = version;
	newRequest.version = rawRequest.substr(0, rawRequest.find('\r'));
	// /r -> almost done
	// /n -> done
	rawRequest = rawRequest.substr(rawRequest.find('\n') + 1);
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
	size_t i = 0;
	while (state != sw_end && i < rawRequest.size()) {
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
			break;
		};
		++i;
	}
	return HttpRequest::PARSE_REQUEST_LINE_DONE;
}

/*
 * read each into hash??
 *
 *
 */
HttpRequest::ParseState HttpRequest::parseHeaders(std::string& rawRequest, HttpRequest& newRequest)
{
	enum parseHeaderPhase {
		sw_start,
		sw_name,
		sw_colon,
		sw_space_before_value,
		sw_value,
		sw_space_after_value,
		sw_header_almost_done,
		sw_header_done,
		sw_end
	} state;

	std::string cur_name;
	std::string cur_value;
	state = sw_start;
	size_t i = 0;
	while (state != sw_end && i < rawRequest.size()) {
		char ch = rawRequest[i];
		switch (state) {
		case sw_start:
			if (!std::isalnum(ch)) state = sw_end;
			else state = sw_name;
			break;
		case sw_name:
			if (!cur_name.empty() && ch == ':') {
				newRequest.headers[cur_name];
				state = sw_colon;
			} else {
				cur_name += ch;
				++i;
			}
			break;
		case sw_colon:
			state = sw_space_before_value;
			++i;
			break;
		case sw_space_before_value:
			if (ch != ' ') state = sw_value;
			else ++i;
			break;
		case sw_value:
			if (!std::isalnum(ch)) {
				newRequest.headers[cur_name] = cur_value;
				cur_name = "";
				cur_value = "";
				state = sw_space_after_value;
			} else {
				cur_value += ch;
				++i;
			}
			break;	
		case sw_space_after_value:
			if (ch != ' ') {
				state = sw_header_almost_done;
			} else {
				++i;
			}
			break;
		case sw_header_almost_done:
			if (ch != '\r')
				return HttpRequest::PARSE_ERROR;
			state = sw_header_done;
			++i;
			break;
		case sw_header_done:
			if (ch != '\n')
				return HttpRequest::PARSE_ERROR;
			state = sw_start;
			++i;
			break;
		default:
			// hmm
			break;
		}
	}
	rawRequest = rawRequest.substr(i + 2);
	return HttpRequest::PARSE_HEADER_DONE;
}

void HttpRequest::parseBody(std::string& body, HttpRequest& newRequest)
{
	newRequest.body = body;
}

std::string HttpRequest::urlDecode(const std::string& str, HttpRequest& newRequest)
{
	(void)str;
	(void)newRequest;
	return "";
}

