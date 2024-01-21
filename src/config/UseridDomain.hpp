#ifndef CONFIG_USERID_DOMAIN_HPP
#define CONFIG_USERID_DOMAIN_HPP

#include <string>

namespace config
{
class UseridDomain
{
	private:
		std::string	name_;
	public:
		UseridDomain() {}
		~UseridDomain() {}
		const static char	*kDefaultName_;
		const std::string	&getName() const { return this->name_; }
		void	setName(const std::string &name) { this->name_ = name; }
};

const char	*UseridDomain::kDefaultName_ = "none";
} // namespace config

#endif