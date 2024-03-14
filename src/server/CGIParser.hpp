#ifndef CGI_CGI_PARSER_HPP
#define CGI_CGI_PARSER_HPP

#include <string>
#include <map>

namespace cgi
{
class CGIParser
{
	private:
		std::map<std::string, std::string>	headers_;
		std::string	body_;
	public:
		CGIParser();
		~CGIParser();
		bool	parse();
};
} // namespace cgi

#endif
