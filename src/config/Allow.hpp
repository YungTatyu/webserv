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
};
} // namespace config

#endif