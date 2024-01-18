#ifndef CONFIG_DENY_PARSER_HPP
#define CONFIG_DENY_PARSER_HPP

#include "conf.hpp"
#include "Lexer.hpp"

namespace config
{
class DenyParser
{
	private:
		DenyParser();
	public:
		~DenyParser();
		const static unsigned int	type_ = CONF_MAIN|CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION // contextの情報
											|CONF_TAKE1; // argsの情報
		/**
		nginxは一つの変数typeでdirectiveの情報をすべて管理している
		以下のようにcontextとargの情報を分けて管理することも可能だが・・・

		const static unsigned int	context_ = CONF_MAIN|CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION;
		const static ARGS	args_ = CONF_TAKE1;
		*/
		static bool	parse(Token &token);
};
} // namespace config


#endif
