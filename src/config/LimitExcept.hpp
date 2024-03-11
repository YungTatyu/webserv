#ifndef CONFIG_LIMITEXCEPT_HPP
#define CONFIG_LIMITEXCEPT_HPP

#include <vector>
#include <set>
#include <string>

#include "Use.hpp"
#include "conf.hpp"
#include "AllowDeny.hpp"

namespace config
{
enum REQUEST_METHOD
{
	UNKNOWN,
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
	std::set<std::string>	directives_set;
	std::set<REQUEST_METHOD>	excepted_methods; // limit_exceptの制限から除外されるmethod
	std::vector<AllowDeny>	allow_deny_list;
};
} // namespace config

#endif
