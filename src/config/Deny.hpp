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
		const std::string	&getAddress() const;
		void	setAddress(const std::string &address);
};
} // namespace config

#endif