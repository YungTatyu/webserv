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
		ErrorLog() : file_(this->kDefaultFile_) {}
		~ErrorLog() {}
		const static char	*kDefaultFile_;
		const std::string	&getFile() const { return this->file_; }
		void	setFile(const std::string &file) { this->file_ = file; }
	};

const char	*ErrorLog::kDefaultFile_ = "logs/error.log";
} // namespace config

#endif