#ifndef CONFIG_DENY_HPP
#define CONFIG_DENY_HPP

#include <string>
#include "conf.hpp"

namespace config
{
class Deny
{
	private:
		std::string	address_;
	public:
		Deny() {}
		~Deny() {}
		const std::string	&getAddress() const { return this->address_; }
		void	setAddress(const std::string &address) { this->address_ = address; }
		const static unsigned int	type_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_HTTP_LIMIT_EXCEPT|CONF_TAKE1;
};
} // namespace config

#endif