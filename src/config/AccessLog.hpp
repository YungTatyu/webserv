#ifndef ACCESS_LOG_HPP
#define ACCESS_LOG_HPP

#include <string>

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
};

const char	*AccessLog::kDefaultFile_ = "logs/access.log";
} // namespace config

#endif