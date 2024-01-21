#ifndef CONFIG_ROOT_HPP
#define CONFIG_ROOT_HPP

#include <string>

namespace config
{
class Root
{
	private:
		std::string	path_;
	public:
		Root() : path_(this->kDefaultPath_) {}
		~Root() {}
		const static char	*kDefaultPath_;
		const std::string	&getPath() const { return this->path_; }
		void	setPath(const std::string &path) { this->path_ = path; }
};

const char	*Root::kDefaultPath_ = "html";
} // namespace config

#endif