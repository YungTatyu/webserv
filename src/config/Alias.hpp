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
		const std::string	&getPath() const;
		void	setPath(const std::string &path);
};	
} // namespace config

#endif