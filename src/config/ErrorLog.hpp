#ifndef CONFIG_ERRORLOG_HPP
#define CONFIG_ERRORLOG_HPP

#include <string>

namespace config
{
class ErrorLog
{
	private:
		std::string	file_;
	public:
		ErrorLog();
		~ErrorLog();
		const static char	*kDefaultFile_ = "logs/error.log";
	};
} // namespace config


#endif