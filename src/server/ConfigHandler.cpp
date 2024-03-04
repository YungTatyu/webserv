#include "ConfigHandler.hpp"
#include "FileUtils.hpp"
#include <sys/socket.h>
#include <cstring>

/** Configにあってほしい機能
 * デフォルトサーバがどれか
 *  ipとmask分けて保存
*/

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

uint32_t	ConfigHandler::StrToIPAddress( const std::string& ip) const
{
	std::istringstream iss(ip);
	std::string segment;
	std::vector<std::string> segments;

	// "." で分割
	while (std::getline(iss, segment, '.')) {
		segments.push_back(segment);
	}

	uint32_t	result = 0;

	for (int i = 0; i < 4; i++)
	{
		iss.clear();
		iss.str(segments[i]);
		int value;
		iss >> value;

		result = (result << 8) | value;
	}

	return result;
}

bool	ConfigHandler::addressInLimit( const std::string& ip_addr_str, const uint32_t cli_addr ) const
{
	std::istringstream	iss(ip_addr_str);
	std::string			ip;
	std::string			mask;

	std::getline(iss, ip, '/');
	std::getline(iss, mask);

	uint32_t			conf_addr = StrToIPAddress(ip);
	uint32_t			mask_val = 0xFFFFFFFF;
	// サブネットマスクが指定されている場合
	if (!mask.empty())
	{
		int prefix_length;
		iss.clear();
		iss.str(mask);
		iss >> prefix_length;
		mask_val <<= (32 - prefix_length);
	}

	return (conf_addr & mask_val) == (cli_addr & mask_val);
}

bool	ConfigHandler::limitLoop( const std::vector<config::AllowDeny>& allow_deny_list, const uint32_t cli_addr ) const
{
	// 上から順に制限適用する
		//制限されているアドレスであれば、false
		//エラーページどのタイミングで返すか？
	for (size_t i = 0; i < allow_deny_list.size(); i++)
	{
		if (addressInLimit(allow_deny_list[i].getAddress(), cli_addr))
		{
			switch (allow_deny_list[i].getAccessDirective())
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

config::REQUEST_METHOD	ConfigHandler::convertRequestMethod( const std::string& method_str ) const
{
	if (method_str == "GET")
		return config::GET;
	else if (method_str == "HEAD")
		return config::HEAD;
	else if (method_str == "POST")
		return config::POST;
	else if (method_str == "PUT")
		return config::PUT;
	else
		return config::DELETE;
}

bool	ConfigHandler::allowRequest( const config::Server& server, const config::Location* location, const HttpRequest& request, struct sockaddr_in client_addr ) const
{
	// ------ access の制限 ------
	// configからアドレス制限ディレクトリのあるcontext探す
	if (location != NULL && location->directives_set.find("deny") != location->directives_set.end())
	{
		if (!limitLoop(location->allow_deny_list, client_addr.sin_addr.s_addr))
			return false;
	}
	else if (server.directives_set.find("deny") != server.directives_set.end())
	{
		if (!limitLoop(server.allow_deny_list, client_addr.sin_addr.s_addr))
			return false;
	}
	else if (this->config_->http.directives_set.find("deny") != this->config_->http.directives_set.end())
	{
		if (!limitLoop(this->config_->http.allow_deny_list, client_addr.sin_addr.s_addr))
			return false;
	}


	// ------ method の制限 ------
	// location内にlimit_except contextあるか？
	if (location->directives_set.find("limit_except") != location->directives_set.end())
	{
		// 制限されたメソッドでなければ、スルー
		// HttpRequestでLIMIT_EXCEPTのenum使ってほしい
		if (location->limit_except.excepted_methods.find(convertRequestMethod(request.method)) == location->limit_except.excepted_methods.end())
		{
			if (!limitLoop(location->limit_except.allow_deny_list, client_addr.sin_addr.s_addr))
				return false;
		}
	}

	// 問題なければtrue
	return true;
}

/** request 処理の順番
 * 1. parse error 400 Bad Request
 * 2. access restrict 403 Invalid Access
 * 3. no location / no file 404 Not Found
 */

const std::string	ConfigHandler::searchFile( const struct config::Server& server, const HttpRequest& request ) const
{
	(void)server;
	(void)request;
	return "";
	//if (FileUtils::isDirectory(server.root.getPath() + request.uri))
	//	return searchErrorPage(301);

	/* ~ try_filesとindex/autoindexのファイル検索 ~
	 * try_filesはlocationのuriを探すファイルのルートにいれずに内部リダイレクト
	 * index/autoindex はrequestのuriにindexのファイル名を足して探す
	 * 3つともなかったら上位のcontextで検索する
	 */

	// try_filesとindex/autoindexをuriが属するcontextから探して返す。見つからなければ403エラー

	// request uriがそもそもrootディレクティブになければ404 Not Found
}

// 最終的なlocationで記録
void	ConfigHandler::writeAcsLog( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	// access_logがどのコンテキスがあれば出力する
	if (location && location->directives_set.find("access_fd") != location->directives_set.end())
	{
		for (size_t i = 0; i < location->access_fd_list.size(); i++)
		{
			write(location->access_fd_list[i], msg.c_str(), msg.length());
		}
	}
	else if (server.directives_set.find("access_fd") != server.directives_set.end())
	{
		for (size_t i = 0; i < server.access_fd_list.size(); i++)
		{
			write(server.access_fd_list[i], msg.c_str(), msg.length());
		}
	}
	else if (this->config_->http.directives_set.find("access_fd") != this->config_->http.directives_set.end())
	{
		for (size_t i = 0; i < this->config_->http.access_fd_list.size(); i++)
		{
			write(this->config_->http.access_fd_list[i], msg.c_str(), msg.length());
		}
	}
}
void	ConfigHandler::writeErrLog( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	// error_logがどのコンテキスがあれば出力する
	if (location && location->directives_set.find("error_fd") != location->directives_set.end())
	{
		for (size_t i = 0; i < location->error_fd_list.size(); i++)
		{
			write(location->error_fd_list[i], msg.c_str(), msg.length());
		}
	}
	else if (server.directives_set.find("error_fd") != server.directives_set.end())
	{
		for (size_t i = 0; i < server.error_fd_list.size(); i++)
		{
			write(server.error_fd_list[i], msg.c_str(), msg.length());
		}
	}
	else if (this->config_->http.directives_set.find("error_fd") != this->config_->http.directives_set.end())
	{
		for (size_t i = 0; i < this->config_->http.error_fd_list.size(); i++)
		{
			write(this->config_->http.error_fd_list[i], msg.c_str(), msg.length());
		}
	}
	else if (this->config_->directives_set.find("error_fd") != this->config_->directives_set.end())
	{
		for (size_t i = 0; i < this->config_->error_fd_list.size(); i++)
		{
			write(this->config_->error_fd_list[i], msg.c_str(), msg.length());
		}
	}
}

const config::Time&	ConfigHandler::searchKeepaliveTimeout( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	if (location && location->directives_set.find("keepalive_timeout") != location->directives_set.end())
	{
		return location->keepalive_timeout.getTime();
	}
	else if (server.directives_set.find("keepalive_timeout") != server.directives_set.end())
	{
		return server.keepalive_timeout.getTime();
	}
	else // http兼default
	{
		return this->config_->http.keepalive_timeout.getTime();
	}
}


const config::Time&	ConfigHandler::searchSendTimeout( const TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	if (location && location->directives_set.find("send_timeout") != location->directives_set.end())
	{
		return location->send_timeout.getTime();
	}
	else if (server.directives_set.find("send_timeout") != server.directives_set.end())
	{
		return server.send_timeout.getTime();
	}
	else // http兼default
	{
		return this->config_->http.send_timeout.getTime();
	}
}

const config::Time&	ConfigHandler::searchUseridExpires( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	if (location && location->directives_set.find("userid_expires") != location->directives_set.end())
	{
		return location->userid_expires.getTime();
	}
	else if (server.directives_set.find("userid_expires") != server.directives_set.end())
	{
		return server.userid_expires.getTime();
	}
	else // http兼default
	{
		return this->config_->http.userid_expires.getTime();
	}
}

const config::Server&	ConfigHandler::searchServerConfig( const struct TiedServer& tied_servers, const std::string& server_name ) const
{
	const config::Server*	default_server = tied_servers.servers_[0];

	for (size_t i = 0; i < tied_servers.servers_.size(); i++)
	{
		if (tied_servers.servers_[i]->server_name.getName().find(server_name) != tied_servers.servers_[i]->server_name.getName().end())
			return *tied_servers.servers_[i];
		// default_server特定できるようにする
		for (size_t j = 0; j < tied_servers.servers_[i]->listen_list.size(); j++)
		{
			const config::Listen& tmp_listen = tied_servers.servers_[i]->listen_list[j];
			if (tmp_listen.getIsDefaultServer() &&
				tied_servers.port_ == tmp_listen.getport() &&
				tied_servers.addr_ == tmp_listen.getAddress())
			{
				default_server = tied_servers.servers_[i];
			}
		}
	}

	// server_nameが一致するものがなければデフォルトサーバーを返す
	return *default_server;
}

bool	sameURI( const std::string& request_uri, std::string config_uri )
{
	// location uriが'/'で始まってなかったらスラッシュをつける
	if (config_uri[0] != '/')
		config_uri.insert(config_uri.begin(), '/');
	// location uriが'/'で終わってなかったらスラッシュをつける
	if (config_uri[config_uri.length() - 1] != '/')
		config_uri.push_back('/');

	if (request_uri == config_uri)
		return true;
	return false;
}

const config::Location*	ConfigHandler::searchLongestMatchLocationConfig( const config::Server& server_config, const std::string& uri ) const
{
	// uriがファイルなら直前の/まで切る
	// でもそのファイルやディレクトリが存在しなかったら location / の内容を探すわけではない

	// location探す
	for (size_t i = 0; i < server_config.location_list.size(); i++)
	{
		if (sameURI(uri, server_config.location_list[i].uri))
			return &server_config.location_list[i];
	}
	return NULL;
}

const config::ErrorPage*	ConfigHandler::searchErrorPage( const config::Server& server, const config::Location* location, const unsigned int code ) const
{
	if (location && !location->error_page_list.empty())
	{
		const std::vector<config::ErrorPage>&	ep_list = location->error_page_list;
		for (size_t i = 0; i < ep_list.size(); i++)
		{
			if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end())
			{
				return &ep_list[i];
			}
		}
	}
	else if (!server.error_page_list.empty())
	{
		const std::vector<config::ErrorPage>&	ep_list = server.error_page_list;
		for (size_t i = 0; i < ep_list.size(); i++)
		{
			if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end())
			{
				return &ep_list[i];
			}
		}
	}
	else if (!this->config_->http.error_page_list.empty())
	{
		const std::vector<config::ErrorPage>&	ep_list = this->config_->http.error_page_list;
		for (size_t i = 0; i < ep_list.size(); i++)
		{
			if (ep_list[i].getCodeList().find(code) != ep_list[i].getCodeList().end())
			{
				return &ep_list[i];
			}
		}
	}

	return NULL;
}

