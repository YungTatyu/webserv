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
		const unsigned int	&get_code() const;
		const std::string	&get_url() const;
		void	set_code(const unsigned int &code);
		void	set_url(const std::string &url);
};
} // namespace config

#endif
