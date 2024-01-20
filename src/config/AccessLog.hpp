#ifndef ACCESS_LOG_HPP
#define ACCESS_LOG_HPP

#include <string>
#include "conf.hpp"

namespace config
{
class AccessLog
{
	private:
		std::string	file_;
	public:
		AccessLog() : file_(this->kDefaultFile_) {}
		~AccessLog() {}
		const std::string	&getFile() const { return this->file_; }
		void	setFile(const std::string	&file) { this->file_ = file; };
		const static char	*kDefaultFile_;
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1;
};

const char	*AccessLog::kDefaultFile_ = "logs/access.log";
} // namespace config

#endif