#ifndef CONFIG_DENY_HPP
#define CONFIG_DENY_HPP

#include <string>

namespace config
{
class Deny
{
	private:
		std::string	address_;
	public:
		Deny();
		~Deny();
		const std::string	&get_address() const;
		void	set_address(const std::string &address);
};
} // namespace config

#endif