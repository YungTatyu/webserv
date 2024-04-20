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
		bool		is_accesslog_on_;
	public:
		AccessLog() : file_(this->kDefaultFile_), is_accesslog_on_(kDefaultIsAccesslogOn) {}
		~AccessLog() {}
		const std::string	&getFile() const { return this->file_; }
		const bool			&getIsAccesslogOn() const { return this->is_accesslog_on_; }
		void	setFile(const std::string	&file) { this->file_ = file; }
		void	setIsAccesslogOn(const bool is_accesslog_on) { this->is_accesslog_on_ = is_accesslog_on; }
		const static char	*kDefaultFile_;
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1|CONF_NOT_UNIQUE;
		const static bool	kDefaultIsAccesslogOn = true;
};
} // namespace config

#endif
