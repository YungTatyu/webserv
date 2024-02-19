#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <sstream>
# include <string>
# include <map>

class HttpRequest
{
	public:
		HttpRequest(const std::string& method = "", const std::string& uri = "", const std::string& version = "", const std::map<std::string, std::string>& headers = std::map<std::string, std::string>(), const std::map<std::string, std::string>& queries = std::map<std::string, std::string>(), const std::string& body = "");
		~HttpRequest();

		static HttpRequest parseRequest(const std::string& rawRequest);
		static void parseChunked(HttpRequest& request);

		std::string method;
		std::string uri;
		std::string version;
		std::map<std::string, std::string> headers;
		std::map<std::string, std::string> queries;
		std::string body;

		bool isParseCompleted; // false when chunked
		bool isParseError;

	private:
		static void parseUri();
		static void parseRequestLine(std::istringstream& requestLine);
		static void parseHeaders(std::istringstream& headers);
		static void parseBody(std::istringstream& body);
		std::string urlDecode(const std::string& str);
};

#endif
