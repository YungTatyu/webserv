#ifndef CONFIG_USERID_DOMAIN_HPP
#define CONFIG_USERID_DOMAIN_HPP

#include <string>

namespace config
{
class UseridPath
{
	private:
		std::string	path_;
	public:
		UseridPath();
		~UseridPath();
		const static char	*kDefaultPath_ = "/";
};
} // namespace config

#endif