#ifndef CONFIG_USERID_DOMAIN_HPP
#define CONFIG_USERID_DOMAIN_HPP

#include <string>

#include "conf.hpp"

namespace config
{
class UseridDomain
{
	private:
		std::string	name_;
	public:
		UseridDomain() {}
		~UseridDomain() {}
		const static char	*kDefaultName_;
		const std::string	&getName() const { return this->name_; }
		void	setName(const std::string &name) { this->name_ = name; }
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1|CONF_UNIQUE;
};
} // namespace config

#endif