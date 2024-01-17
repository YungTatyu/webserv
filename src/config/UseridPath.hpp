#ifndef CONFIG_USERID_PATH_HPP
#define CONFIG_USERID_PATH_HPP

#include <string>

namespace config
{
class UseridPath
{
	private:
		std::string	path_;
	public:
		UseridPath() : path_(kDefaultPath_) {}
		~UseridPath() {}
		const static char	*kDefaultPath_;
		const std::string	&getPath() const { return this->path_; }
		void	setPath(const std::string &path) { this->path_ = path; }
};

const char	*kDefaultPath_ = "/";
} // namespace config

#endif