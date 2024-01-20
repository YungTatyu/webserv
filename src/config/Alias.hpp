#ifndef CONFIG_ALIAS_HPP
#define CONFIG_ALIAS_HPP

#include <string>

#include <conf.hpp>

namespace config
{
class Alias
{
	private:
		std::string	path_;
	public:
		Alias() {}
		~Alias() {}
		const std::string	&getPath() const { return this->path_; }
		void	setPath(const std::string &path) {this->path_ = path; };
		const static unsigned int	kType_ = CONF_HTTP_LOCATION|CONF_TAKE1;
};
} // namespace config

#endif