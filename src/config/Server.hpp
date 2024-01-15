#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <vector>

#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Allow.hpp"
#include "Deny.hpp"
#include "Index.hpp"

namespace config
{
struct Server
{
	std::vector<AccessLog>	access_log_list;
	std::vector<ErrorLog>	error_log_list;
	std::vector<ErrorPage>	error_page_list;
	std::vector<Allow>	allow_list;
	std::vector<Deny>	deny_list;
	std::vector<Index>	index_list;

};

} // namespace config


#endif
