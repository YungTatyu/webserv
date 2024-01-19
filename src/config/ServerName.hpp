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
		ServerName() {}
		~ServerName() {}
		const static char	*kDefaultNmae_ = "";
		const std::string	&getName() const;
		void	setName(const std::string &name);
};
} // namespace config

#endif