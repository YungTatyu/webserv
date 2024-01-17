#ifndef CONFIG_LIMITEXCEPT_HPP
#define CONFIG_LIMITEXCEPT_HPP

#include <vector>

#include "Use.hpp"
#include "Allow.hpp"
#include "Deny.hpp"

namespace config
{
class LimitExcept
{
	public:
		LimitExcept(Use::CONNECTION_METHOD method) : excepted_method_(method) {}
		~LimitExcept();
		Use::CONNECTION_METHOD	excepted_method_; // limit_exceptの制限から除外されるmethod
		std::vector<Allow>	allow_list;
		std::vector<Deny>	deny_list;
};
} // namespace config

#endif
