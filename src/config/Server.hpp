#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <vector>

#include "AccessLog.hpp"
#include "ErrorLog.hpp"
#include "ErrorPage.hpp"
#include "Allow.hpp"
#include "Deny.hpp"
#include "Index.hpp"
#include "KeepaliveTimeout.hpp"
#include "Listen.hpp"
#include "Root.hpp"
#include "ServerName.hpp"
#include "TryFiles.hpp"
#include "Userid.hpp"
#include "UseridDomain.hpp"
#include "UseridExpires.hpp"
#include "UseridPath.hpp"
#include "UseridService.hpp"

namespace config
{
class Server
{
	public:
		Server() {}
		~Server() {}
		std::vector<AccessLog>	access_log_list;
		std::vector<ErrorLog>	error_log_list;
		std::vector<ErrorPage>	error_page_list;
		std::vector<Allow>	allow_list;
		std::vector<Deny>	deny_list;
		std::vector<Index>	index_list;
		std::vector<Listen>	listen_list;
		KeepaliveTimeout	keepalive_timeout;
		Root	root;
		std::vector<ServerName>	server_name_list;
		TryFiles	*try_files; // defaultがnoneなので、configで設定されない限り値を持ちたくない：初期値NULL
		Userid	userid;
		UseridDomain	*userid_domain; // defaultがnoneなので、configで設定されない限り値を持ちたくない：初期値NULL
		UseridExpires	userid_expires;
		UseridPath	userid_path;
		UseridService	user_service;
};

} // namespace config


#endif
