#ifndef CONFIG_ALIAS_HPP
#define CONFIG_ALIAS_HPP

#include <string>

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
};	
} // namespace config

#endif