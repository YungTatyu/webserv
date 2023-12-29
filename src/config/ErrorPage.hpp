#ifndef CONFIG_ERROR_PAGE_HPP
#define CONFIG_ERROR_PAGE_HPP

#include <string>

namespace config
{
class ErrorPage
{
	private:
		unsigned int	code;
		std::string	uri;
	public:
		ErrorPage();
		~ErrorPage();
};
} // namespace config


#endif