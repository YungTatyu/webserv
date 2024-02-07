#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <sstream>
# include <string>
# include <map>

class HttpRequest
{
	public:
		HttpRequest();
		~HttpRequest();

		static HttpRequest parseRequest(const std::string& rawRequest);

		std::string method;
		std::string uri;
		std::string version;
		std::map<std::string, std::string> headers;
		std::map<std::string, std::string> queries;
		std::string body;

	private:
		void parseUri();
		void parseRequestLine(std::istringstream& requestLine);
		void parseHeaders(std::istringstream& headers);
		void parseBody(std::istringstream& body);
		std::string urlDecode(const std::string& str);
		//chunked??
};

#endif
