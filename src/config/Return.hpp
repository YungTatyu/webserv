#ifndef CONFIG_RETURN_HPP
#define CONFIG_RETURN_HPP

#include <string>

namespace config
{
class Return
{
	private:
		unsigned int	code_;
		std::string	url_; // url or text
	public:
		Return() {}
		~Return() {}
};
} // namespace config

#endif
