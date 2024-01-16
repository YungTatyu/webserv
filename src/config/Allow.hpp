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
		const std::string	&getAddress() const;
		void	setAddress(const std::string &address);
};
} // namespace config

#endif