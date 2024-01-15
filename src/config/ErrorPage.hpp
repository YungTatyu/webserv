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
		unsigned int	response_;
		std::string	uri_;
	public:
		ErrorPage();
		~ErrorPage();
		const std::vector<unsigned int>	&get_code_list() const;
		const unsigned int	&get_response() const;
		const std::string	&get_uri() const;
		void	add_code(const unsigned int &code);
		void	set_response(const unsigned int &response);
		void	set_uri(const std::string &uri);
};
} // namespace config


#endif