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
		const std::string	&getAddress() const { return this->address_; }
		void	setAddress(const std::string &address) { this->address_ = address; }
};
} // namespace config

#endif