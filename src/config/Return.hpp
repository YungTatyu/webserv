#ifndef CONFIG_RETURN_HPP
#define CONFIG_RETURN_HPP

#include <string>

#include "conf.hpp"

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
		const unsigned int	&getCode() const { return this->code_; }
		const std::string	&getUrl() const { return this->url_; }
		void	setCode(const unsigned int &code) { this->code_ = code; }
		void	setUrl(const std::string &url) { this->url_ = url; }
		const static unsigned int	type_ = CONF_HTTP_LOCATION|CONF_TAKE12;
};
} // namespace config

#endif
