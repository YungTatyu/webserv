#ifndef CONFIG_ROOT_HPP
#define CONFIG_ROOT_HPP

#include <string>

#include "conf.hpp"

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
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|
											CONF_TAKE1|CONF_UNIQUE;
};
} // namespace config

#endif