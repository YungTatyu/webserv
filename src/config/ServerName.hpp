#ifndef CONFIG_SERVER_NAME_HPP
#define CONFIG_SERVER_NAME_HPP

#include <string>

namespace config
{
class ServerName
{
	private:
		std::string	name_;
	public:
		ServerName() : name_(this->kDefaultNmae_) {}
		~ServerName() {}
		const static char	*kDefaultNmae_;
		const std::string	&getName() const { return this->name_; }
		void	setName(const std::string &name) { this->name_ = name; }
};

const char	*ServerName::kDefaultNmae_ = "";
} // namespace config

#endif