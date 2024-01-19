#ifndef CONFIG_LIMITEXCEPT_HPP
#define CONFIG_LIMITEXCEPT_HPP

#include <vector>

#include "Allow.hpp"
#include "Deny.hpp"

namespace config
{
struct LimitExcept
{
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
};
} // namespace config

#endif
