#ifndef CONFIG_LIMITEXCEPT_HPP
#define CONFIG_LIMITEXCEPT_HPP

#include <vector>
#include <set>
#include <string>

#include "Use.hpp"
#include "conf.hpp"
#include "Allow.hpp"
#include "Deny.hpp"

namespace config
{
struct LimitExcept
{
	LimitExcept(const std::string &method) : excepted_method_(method) {}
	~LimitExcept() {}
	const static unsigned int	type = CONF_HTTP_LOCATION|CONF_TAKE1|CONF_NOT_UNIQUE;
	std::set<std::string>	set_directives;
	const std::string	excepted_method_; // limit_exceptの制限から除外されるmethod
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
};
} // namespace config

#endif
