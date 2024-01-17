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
		Root();
		~Root();
		const static char	*kDefaultPath_ = "html";
};
} // namespace config


#endif