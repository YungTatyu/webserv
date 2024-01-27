#ifndef CONFIG_ALLOW_HPP
#define CONFIG_ALLOW_HPP

#include <string>
#include "conf.hpp"

namespace config
{
class Allow
{
	private:
		std::string	address_;
	public:
		Allow() {}
		~Allow() {}
		const std::string	&getAddress() const { return this->address_; };
		void	setAddress(const std::string &address) {this->address_ = address; };
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_HTTP_LIMIT_EXCEPT|CONF_TAKE1|CONF_NOT_UNIQUE;
};
} // namespace config

#endif