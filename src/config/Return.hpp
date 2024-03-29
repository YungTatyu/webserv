#ifndef CONFIG_RETURN_HPP
#define CONFIG_RETURN_HPP

#include <string>
#include <algorithm>

#include "conf.hpp"

namespace config
{
class Return
{
	private:
		int			code_;
		std::string	url_; // url or text
		const static int	kRedirectCodesSize = 5;
	public:
		Return() : code_(this->kCodeUnset) {}
		~Return() {}
		const int	&getCode() const { return this->code_; }
		const std::string	&getUrl() const { return this->url_; }
		void	setCode(const int &code) { this->code_ = code; }
		void	setUrl(const std::string &url) { this->url_ = url; }
		static bool	isRedirectCode(const int code)
		{
			for (size_t i = 0; i < kRedirectCodesSize; ++i)
			{
				if (kRedirectCodes_[i] == code)
					return true;
			}
			return false;
		};
		const static unsigned int	kType_ = CONF_HTTP_LOCATION|CONF_TAKE12|CONF_NOT_UNIQUE;
		const static int	kCodeUnset = -1;
		const static int	kRedirectCodes_[kRedirectCodesSize];
};
} // namespace config

#endif
