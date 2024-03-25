#ifndef CGI_CGI_PARSER_HPP
#define CGI_CGI_PARSER_HPP

#include <string>
#include <map>

#include "HttpResponse.hpp"
#include "Utils.hpp"

namespace cgi
{
enum PARSE_STATE
{
	PARSE_BEFORE = 0,
	PARSE_COMPLETE,
	PARSE_INPROGRESS,
	PARSE_HEADER_DONE,
	PARSE_BODY_DONE,
	PARSE_ERROR
};

typedef std::map<std::string, std::string, Utils::CaseInsensitiveCompare> string_map_case_insensitive;

class CGIParser
{
	private:
		// 以下のメンバ変数はHttpResponseのメンバ変数のポインタ
		string_map_case_insensitive	*headers_;
		std::string	*body_;
		long	*status_code_;
		std::string	*status_code_line_;
		size_t	cri_; // cgi response index
		void	parseHeaders(const std::string& cgi_response);
		void	parseBody(const std::string& cgi_response);
		bool	isValidStatusCode(const std::string& status_code) const;
		void	setStatusCode(const std::string& value);
	public:
		CGIParser();
		~CGIParser();
		PARSE_STATE	state_;
		void	init(HttpResponse& http_response);
		bool	parse(HttpResponse& http_response, const std::string& cgi_response, const PARSE_STATE init_state);
};
} // namespace cgi

#endif
