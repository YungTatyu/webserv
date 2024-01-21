#ifndef CONFIG_ALLOW_HPP
#define CONFIG_ALLOW_HPP

#include <string>

namespace config
{
class Allow
{
	private:
		std::string	address_;
	public:
		Allow();
		~Allow();
		const std::string	&getAddress() const { return this->address_; };
		void	setAddress(const std::string &address) {this->address_ = address; };
};
} // namespace config

#endif