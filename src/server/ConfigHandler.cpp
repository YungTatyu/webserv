#include "ConfigHandler.hpp"
#include <sys/socket.h>

/* ConfigHandlerクラスの実装 */
void ConfigHandler::loadConfiguration( const config::Main* config )
{
	/* ConfファイルをパースしてデータをServConfigクラスにセットする */
	this->config_ = config;
	this->servPort_ = 3001;
	this->listenQ_ = 8;
}

int ConfigHandler::getServPort()
{
	return this->servPort_;
}

int ConfigHandler::getListenQ()
{
	return this->listenQ_;
}

uint32_t	ConfigHandler::StrToIpAddress( const std::string& ip)
{
	std::istringstream iss(ip);
	std::string segment;
	std::vector<std::string> segments;

	// "." で分割
	while (std::getline(iss, segment, '.')) {
		segments.push_back(segment);
	}

	uint32_t result = 0;

	for (int i = 0; i < 4; i++)
	{
		int value = std::stoi(segments[i]);

		result = (result << 8) | value;
	}

	return result;
}

bool	ConfigHandler::addressInLimit( const std::string& ip_addr_str, const uint32_t cli_addr )
{
	std::istringstream	iss(ip_addr_str);
	std::string			ip;
	std::string			mask;

	std::string std::getline(iss, ip, '/');
	std::string std::getline(iss, mask);

	uint32_t			conf_addr = StrToIPAddress(ip);
	uint32_t			mask_val = 0xFFFFFFFF;
	// サブネットマスクが指定されている場合
	if (!mask.empty())
	{
		int prefix_length = std::stoi(mask);
		mask_val <<= (32 - prefix_length);
	}

	return (ip & mask_val) == (cli_addr & mask_val);
}

bool	ConfigHandler::limitLoop( const std::vector<config::AllowDeny>& allow_deny_list, const uint32_t cli_addr )
{
	// 上から順に制限適用する
		//制限されているアドレスであれば、false
		//エラーページどのタイミングで返すか？
	for (int i = 0; i < allow_deny_list.size(); i++)
	{
		if (addressInLimit(allow_deny_list[i].getAddress(), cli_addr))
		{
			switch (allow_deny_list[i].getAllowDirective())
			{
				case config::ALLOW:
					return true;
					break;
				case config::DENY:
					return false;
					break;
				default:
					return false;
					break;
			}
		}
	}

	return true;
}

// parseが失敗したときはその情報どう受け取るか
bool	ConfigHandler::allowRequest( const struct TiedServer& server_config, const HttpRequest& request, const int cli_sock ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);

	// cli_Sockからアドレス読み取る
	struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
	if (getsockname(cli_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addrlen) != 0)
	{
		std::cerr << "webserv: [emerge] getsockname() \"" << cli_sock << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
	}
	//std::cout << "" << client_addr.sin_addr.s_addr << std::endl;

	// ------ access の制限 ------
	// configからアドレス制限ディレクトリのあるcontext探す
	if (location.directives_set[kDENY])
	{
		if (!limitLoop(location.allow_deny_list, client_addr.sin_addr.s_addr))
			return false;
	}
	else if (server.directives_set[kDENY])
	{
		if (!limitLoop(server.allow_deny_list, client_addr.sin_addr.s_addr))
			return false;
	}
	else if (this->config_.http.directives_set[kDENY])
	{
		if (!limitLoop(this->config_.http.allow_deny_list, client_addr.sin_addr.s_addr))
			return false;
	}


	// ------ method の制限 ------
	// location内にlimit_except contextあるか？
	if (location.directives_set(LIMIT_EXCEPT)
	{
		// あれば上から適用する
		for (int i = 0; i < location.limit_except.size(), i++)
		{
		// 制限されたメソッドでなければ、スルー
			if (request.getMethod() == location.limit_except[i].excepted_methods)
			{
				if (!limitLoop(location.limit_except[i].allow_deny_list, client_addr.sin_addr.s_addr))
					return false;
			}
		}
	}

	// 問題なければtrue
	return true;
}


	const std::string&	ConfigHandler::searchFile( const struct TiedServer& server_config, const HttpRequest& request ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);

	// parseが失敗していれば、400 Bad Request

	// allowReuestがfalseなら403 Forbidden


	// urlのlocationがあるか探す
		// なければ上位のcontextから探す
		// あればlocationから探す

	// ファイルが存在するか
		// あれば返す
		// なければ
			// error_pageあればそれ返す
			// なければデフォルト404エラーページ返す
}

void	ConfigHandler::writeAcsLog( const struct TiedServer& server_config, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);

	// access_logがどのコンテキストにあるか
	//
	// access_logのパスすべてに
}
void	ConfigHandler::writeErrLog( const struct TiedServer& server_config, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);
}

const config::Time&	ConfigHandler::getKeepaliveTimeout( const struct TiedServer& server_config, const std::string& server_name, const std::string& uri ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);
}

const config::Time&	ConfigHandler::getSendTimeout( const struct TiedServer& server_config, const std::string& server_name, const std::string& uri ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);
}

const config::Time&	ConfigHandler::getUseridExpires( const struct TiedServer& server_config, const std::string& server_name, const std::string& uri ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);
}

const config::Server&	ConfigHandler::searchServerConfig( const struct TiedServer& server_configs, const std::string& server_name ) const
{
	// Serverクラスにどれがデフォルトサーバーかわかる変数付ける
	for (int i = 0; i < server_configs.size(); i++)
	{
		if (server_name == server_configs.tied_servers_[i].server_name.getName())
			return server_configs.tied_servers[i];
	}
	// return default_server
}

const config::Location&	ConfigHandler::searchLocationConfig( const config::Server& server_config, const std::string& uri ) const
{
	for (int i = 0; i < server_config.location_list.size(); i++)
	{
		if (uri == server_config.location_list[i].uri)
			return server_config.location_list[i];
	}
	// return 404 Not Found
}

const std::string&	ConfigHandler::searchErrorPage( const config::Server& server, const config::Location& location, const unsigned int code )
{
}
