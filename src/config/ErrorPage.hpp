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
		const std::vector<unsigned int>	&getCodeList() const;
		const unsigned int	&getResponse() const;
		const std::string	&getUri() const;
		void	addCode(const unsigned int &code);
		void	setResponse(const unsigned int &response);
		void	setUri(const std::string &uri);
};
} // namespace config


#endif