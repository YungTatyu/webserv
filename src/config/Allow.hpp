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
		const std::string	&get_address() const;
		void	set_address(const std::string &address);
};
} // namespace config

#endif