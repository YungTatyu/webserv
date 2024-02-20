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

bool	ConfigHandler::allowRequest( const struct TiedServer& server_config, const HttpRequest& request, const int cli_sock ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);

}


const std::string&	ConfigHandler::searchFile( const struct TiedServer& server_config, const HttpRequest& request ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);

}

void	ConfigHandler::writeAcsLog( const struct TiedServer& server_config, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location&	location = searchLocationConfig(server, uri);

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
