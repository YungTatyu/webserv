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
		const std::string	&get_path() const;
		void	set_path(const std::string &path);
};	
} // namespace config

#endif