#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <sstream>
# include <string>
# include <map>

class HttpRequest
{
	public:
		enum ParseState
		{
			PARSE_BEFORE,
			PARSE_COMPLETE,
			PARSE_INPROGRESS,
			PARSE_ERROR
		};

		HttpRequest(const std::string& method = "", const std::string& uri = "", const std::string& version = "", const std::map<std::string, std::string>& headers = std::map<std::string, std::string>(), const std::map<std::string, std::string>& queries = std::map<std::string, std::string>(), const std::string& body = "", const ParseState parseState = PARSE_BEFORE);
		~HttpRequest();

		static HttpRequest parseRequest(const std::string& rawRequest);
		static void parseChunked(HttpRequest& request);

		std::string method;
		std::string uri;
		std::string version;
		std::map<std::string, std::string> headers;
		std::map<std::string, std::string> queries;
		std::string body;

		ParseState parseState;

	private:
		static void parseUri();
		static void parseRequestLine(std::istringstream& requestLine);
		static void parseHeaders(std::istringstream& headers);
		static void parseBody(std::istringstream& body);
		std::string urlDecode(const std::string& str);
};

#endif
