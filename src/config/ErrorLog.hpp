#ifndef CONFIG_ERRORLOG_HPP
#define CONFIG_ERRORLOG_HPP

#include <string>
#include "conf.hpp"

namespace config
{
class ErrorLog
{
	private:
		std::string	file_;
	public:
		ErrorLog() : file_(this->kDefaultFile_) {}
		~ErrorLog() {}
		const static char	*kDefaultFile_;
		const std::string	&getFile() const { return this->file_; }
		void	setFile(const std::string &file) { this->file_ = file; }
		const static unsigned int	kType_ = CONF_MAIN|CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|
											CONF_TAKE1|CONF_NOT_UNIQUE;
	};

const char	*ErrorLog::kDefaultFile_ = "logs/error.log";
} // namespace config

#endif