#ifndef CGI_CGI_PARSER_HPP
#define CGI_CGI_PARSER_HPP

#include <string>
#include <map>

#include "HttpResponse.hpp"

namespace cgi
{
class CGIParser
{
	private:
		// 以下のメンバ変数はHttpResponseのメンバ変数のポインタ
		std::map<std::string, std::string>	*headers_;
		std::string	*body_;
		long	*status_code_;
		std::string	*status_code_line_;
		bool	parseHeaders(const std::string &cgi_response);
		bool	parseBody(const std::string &cgi_response);
	public:
		CGIParser();
		~CGIParser();
		void	init(HttpResponse& http_response);
		bool	parse(HttpResponse& http_response, const std::string &cgi_response);
};
} // namespace cgi

#endif
