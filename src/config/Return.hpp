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
		const unsigned int	&getCode() const;
		const std::string	&getUrl() const;
		void	setCode(const unsigned int &code);
		void	setUrl(const std::string &url);
};
} // namespace config

#endif
