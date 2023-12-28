#ifndef CONFIG_ERRORLOG_HPP
#define CONFIG_ERRORLOG_HPP

#include <string>

namespace config
{
class ErrorLog
{
	private:
		std::string	file;
	public:
		ErrorLog();
		~ErrorLog();
	};
} // namespace config


#endif