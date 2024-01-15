#ifndef CONFIG_ERROR_PAGE_HPP
#define CONFIG_ERROR_PAGE_HPP

#include <vector>
#include <string>

namespace config
{
class ErrorPage
{
	private:
		std::vector<unsigned int>	code_list_;
		std::string	uri_;
	public:
		ErrorPage();
		~ErrorPage();
};
} // namespace config


#endif