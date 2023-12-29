#ifndef CONFIG_ERROR_LOG_PARSER_HPP
#define CONFIG_ERROR_LOG_PARSER_HPP

#include "conf.hpp"
#include "Lexer.hpp"

namespace config
{
class ErrorLogParser
{
	private:
		ErrorLogParser();
	public:
		~ErrorLogParser();
		const static unsigned int	context_ = CONF_MAIN|CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION;
		const static ARGS	args_ = CONF_TAKE1;
		static bool	parse(Token &token);
};
} // namespace config


#endif