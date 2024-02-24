#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <sstream>
# include <string>
# include <map>

#define UNKNOWN	0b0000000
#define GET	0b0000001
#define HEAD	0b0000010
#define POST	0b0000100

class HttpRequest
{
	public:
		enum ParseState
		{
			PARSE_BEFORE,
			PARSE_COMPLETE,
			PARSE_INPROGRESS,
			PARSE_METHOD_DONE,
			PARSE_REQUEST_LINE_DONE,
			PARSE_URI_DONE,
			PARSE_VERSION_DONE,
			PARSE_HEADER_DONE,
			PARSE_BODY_DONE,
			PARSE_ERROR
		};

		HttpRequest(const unsigned int method = UNKNOWN, const std::string& uri = "", const std::string& version = "",
			    const std::map<std::string, std::string>& headers = std::map<std::string, std::string>(),
			    const std::map<std::string, std::string>& queries = std::map<std::string, std::string>(),
			    const std::string& body = "",
			    const ParseState parseState = PARSE_BEFORE);
		~HttpRequest();

		static HttpRequest parseRequest(const std::string& rawRequest);
		static void parseChunked(HttpRequest& request);

		unsigned int method;
		std::string uri; // スキーマ、ポートは？？
		std::string version;
		std::map<std::string, std::string> headers; // hashにするためには、unordered_mapを使った方がいい。mapは赤黒木なので計算量logN.hashは最悪O(N)だけど基本O(1).
		std::map<std::string, std::string> queries;
		std::string body;

		ParseState parseState;

	private:
		static ParseState parseMethod(std::string method, HttpRequest& newRequest);
		static ParseState parseUri(std::string uri, HttpRequest& newRequest);
		static ParseState parseVersion(std::string version, HttpRequest& newRequest);
		static ParseState parseRequestLine(const std::string& requestLine, HttpRequest& newRequest);
		static ParseState parseHeaders(const std::string& headers, HttpRequest& newRequest);
		static void parseBody(std::istringstream& body,  HttpRequest& newRequest);
		std::string urlDecode(const std::string& str, HttpRequest& newRequest);
};

#endif
