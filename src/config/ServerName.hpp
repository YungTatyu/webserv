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
		const std::string	&get_name() const;
		void	set_name(const std::string &name);
};
} // namespace config

#endif