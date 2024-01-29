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
enum REQUEST_METHOD
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
};

struct LimitExcept
{
	LimitExcept() {}
	~LimitExcept() {}
	const static unsigned int	type = CONF_HTTP_LOCATION|CONF_1MORE|CONF_UNIQUE;
	std::set<std::string>	set_directives;
	std::set<REQUEST_METHOD>	excepted_methods_; // limit_exceptの制限から除外されるmethod
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
};
} // namespace config

#endif
