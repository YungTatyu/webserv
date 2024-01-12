#ifndef CONFIG_USERID_DOMAIN_HPP
#define CONFIG_USERID_DOMAIN_HPP

#include <string>

namespace config
{
class UseridDomain
{
	private:
		std::string	name;
	public:
		UseridDomain();
		~UseridDomain();
		const static char	*kDefaultName = "none";
};
} // namespace config

#endif