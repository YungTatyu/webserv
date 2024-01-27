#ifndef CONFIG_SERVER_NAME_HPP
#define CONFIG_SERVER_NAME_HPP

#include <string>

#include "conf.hpp"

namespace config
{
class ServerName
{
	private:
		std::string	name_;
	public:
		ServerName() : name_(this->kDefaultName_) {}
		~ServerName() {}
		const static char	*kDefaultName_;
		const std::string	&getName() const { return this->name_; }
		void	setName(const std::string &name) { this->name_ = name; }
		const static unsigned int	kType_ = CONF_HTTP_SERVER|CONF_1MORE|CONF_NOT_UNIQUE;
};
} // namespace config

#endif