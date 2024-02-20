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
		printError();
	}
	std::cout << "" << client_addr.sin_addr.s_addr << std::endl;

	// ------ access の制限 ------
	// configからアドレス制限ディレクトリのあるcontext探す
	if (location.directives_set[kDENY])
	{
		// 関数分割
		// 上から順に制限適用する
			//制限されているアドレスであれば、false
			//エラーページどのタイミングで返すか？
		for (int i = 0; i < location.allow_deny_list.size(); i++)
		{
			if (addressInLimit())
			{
				switch (location.allow_deny_list[i].getAllowDirective()) {
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
	}
	else if (server.directives_set[kDENY])
	{
	}
	else if (this->config_.http.directives_set[kDENY])
	{
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
				// 上から順に適用していく
				for (int j = 0; j < location.limit_except[i].allow_deny_list.size(), j++)
				{
					if (addressInLimits())
					{
						switch (location.limit_except[i].allow_deny_list[j].getAccessDirective()) {
							case config::ALLOW:
								return true;
								break;
							case config::DENY:
								return false;
								break;
							default :
								return false;
								break;
						}
					}
				}
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
