#ifndef CONFIG_ERROR_PAGE_HPP
#define CONFIG_ERROR_PAGE_HPP

#include <vector>
#include <string>

#include "conf.hpp"

namespace config
{
class ErrorPage
{
	private:
		std::vector<unsigned int>	code_list_;
		unsigned int	response_;
		std::string	uri_;
	public:
		ErrorPage() {}
		~ErrorPage() {}
		const std::vector<unsigned int>	&getCodeList() const { return this->code_list_; }
		const unsigned int	&getResponse() const { return this->response_; }
		const std::string	&getUri() const { return this->uri_; }
		void	addCode(const unsigned int &code) { this->code_list_.push_back(code); }
		void	setResponse(const unsigned int &response) { this->response_ = response; }
		void	setUri(const std::string &uri) { this->uri_ = uri; }
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_1MORE;
};
} // namespace config


#endif