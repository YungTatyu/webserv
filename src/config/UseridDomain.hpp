#ifndef CONFIG_USERID_DOMAIN_HPP
#define CONFIG_USERID_DOMAIN_HPP

#include <string>

namespace config
{
class UseridDomain
{
	private:
		std::string	name_;
	public:
		UseridDomain();
		~UseridDomain();
		const static char	*kDefaultName = "none";
		const std::string	&getName() const;
		void	setName(const std::string &name);
};
} // namespace config

#endif