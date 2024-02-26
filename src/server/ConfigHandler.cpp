#include "ConfigHandler.hpp"
#include "FileUtils.hpp"
#include <sys/socket.h>
#include <cstring>

/** Configにあってほしい機能
 * デフォルトサーバがどれか
 *  ipとmask分けて保存
*/

static std::string webserv_error_301_page =
"<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved Permanently</h1></center>\r\n";


static std::string webserv_error_302_page =
"<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n";


static std::string webserv_error_303_page =
"<html>\r\n<head><title>303 See Other</title></head>\r\n<body>\r\n<center><h1>303 See Other</h1></center>\r\n";


static std::string webserv_error_307_page =
"<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary Redirect</h1></center>\r\n";


static std::string webserv_error_308_page =
"<html>\r\n<head><title>308 Permanent Redirect</title></head>\r\n<body>\r\n<center><h1>308 Permanent Redirect</h1></center>\r\n";

static std::string webserv_error_400_page =
"<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n";

static std::string webserv_error_401_page =
"<html>\r\n<head><title>401 Authorization Required</title></head>\r\n<body>\r\n<center><h1>401 Authorization Required</h1></center>\r\n";

static std::string webserv_error_402_page =
"<html>\r\n<head><title>402 Payment Required</title></head>\r\n<body>\r\n<center><h1>402 Payment Required</h1></center>\r\n";

static std::string webserv_error_403_page =
"<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n";

static std::string webserv_error_404_page =
"<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not Found</h1></center>\r\n";

static std::string webserv_error_405_page =
"<html>\r\n<head><title>405 Not Allowed</title></head>\r\n<body>\r\n<center><h1>405 Not Allowed</h1></center>\r\n";

static std::string webserv_error_406_page =
"<html>\r\n<head><title>406 Not Acceptable</title></head>\r\n<body>\r\n<center><h1>406 Not Acceptable</h1></center>\r\n";

static std::string webserv_error_408_page =
"<html>\r\n<head><title>408 Request Time-out</title></head>\r\n<body>\r\n<center><h1>408 Request Time-out</h1></center>\r\n";

static std::string webserv_error_409_page =
"<html>\r\n<head><title>409 Conflict</title></head>\r\n<body>\r\n<center><h1>409 Conflict</h1></center>\r\n";

static std::string webserv_error_410_page =
"<html>\r\n<head><title>410 Gone</title></head>\r\n<body>\r\n<center><h1>410 Gone</h1></center>\r\n";

static std::string webserv_error_411_page =
"<html>\r\n<head><title>411 Length Required</title></head>\r\n<body>\r\n<center><h1>411 Length Required</h1></center>\r\n";

static std::string webserv_error_412_page =
"<html>\r\n<head><title>412 Precondition Failed</title></head>\r\n<body>\r\n<center><h1>412 Precondition Failed</h1></center>\r\n";

static std::string webserv_error_413_page =
"<html>\r\n<head><title>413 Request Entity Too Large</title></head>\r\n<body>\r\n<center><h1>413 Request Entity Too Large</h1></center>\r\n";

static std::string webserv_error_414_page =
"<html>\r\n<head><title>414 Request-URI Too Large</title></head>\r\n<body>\r\n<center><h1>414 Request-URI Too Large</h1></center>\r\n";

static std::string webserv_error_415_page =
"<html>\r\n<head><title>415 Unsupported Media Type</title></head>\r\n<body>\r\n<center><h1>415 Unsupported Media Type</h1></center>\r\n";

static std::string webserv_error_416_page =
"<html>\r\n<head><title>416 Requested Range Not Satisfiable</title></head>\r\n<body>\r\n<center><h1>416 Requested Range Not Satisfiable</h1></center>\r\n";

static std::string webserv_error_421_page =
"<html>\r\n<head><title>421 Misdirected Request</title></head>\r\n<body>\r\n<center><h1>421 Misdirected Request</h1></center>\r\n";

static std::string webserv_error_429_page =
"<html>\r\n<head><title>429 Too Many Requests</title></head>\r\n<body>\r\n<center><h1>429 Too Many Requests</h1></center>\r\n";

static std::string webserv_error_494_page =
"<html>\r\n<head><title>400 Request Header Or Cookie Too Large</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>Request Header Or Cookie Too Large</center>\r\n";


static std::string webserv_error_495_page =
"<html>\r\n<head><title>400 The SSL certificate error</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>The SSL certificate error</center>\r\n";

static std::string webserv_error_496_page =
"<html>\r\n<head><title>400 No required SSL certificate was sent</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>No required SSL certificate was sent</center>\r\n";

static std::string webserv_error_497_page =
"<html>\r\n<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>\r\n<body>\r\n<center><h1>400 Bad Request</h1></center>\r\n<center>The plain HTTP request was sent to HTTPS port</center>\r\n";


static std::string webserv_error_500_page =
"<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body>\r\n<center><h1>500 Internal Server Error</h1></center>\r\n";

static std::string webserv_error_501_page =
"<html>\r\n<head><title>501 Not Implemented</title></head>\r\n<body>\r\n<center><h1>501 Not Implemented</h1></center>\r\n";

static std::string webserv_error_502_page =
"<html>\r\n<head><title>502 Bad Gateway</title></head>\r\n<body>\r\n<center><h1>502 Bad Gateway</h1></center>\r\n";

static std::string webserv_error_503_page =
"<html>\r\n<head><title>503 Service Temporarily Unavailable</title></head>\r\n<body>\r\n<center><h1>503 Service Temporarily Unavailable</h1></center>\r\n";

static std::string webserv_error_504_page =
"<html>\r\n<head><title>504 Gateway Time-out</title></head>\r\n<body>\r\n<center><h1>504 Gateway Time-out</h1></center>\r\n";

static std::string webserv_error_505_page =
"<html>\r\n<head><title>505 HTTP Version Not Supported</title></head>\r\n<body>\r\n<center><h1>505 HTTP Version Not Supported</h1></center>\r\n";

static std::string webserv_error_507_page =
"<html>\r\n<head><title>507 Insufficient Storage</title></head>\r\n<body>\r\n<center><h1>507 Insufficient Storage</h1></center>\r\n";

ConfigHandler::ConfigHandler()
	: config_(NULL)
{
	this->default_page_map_[300] = "";
	this->default_page_map_[301] = webserv_error_301_page;
	this->default_page_map_[302] = webserv_error_302_page;
	this->default_page_map_[303] = webserv_error_303_page;
	this->default_page_map_[307] = webserv_error_307_page;
	this->default_page_map_[308] = webserv_error_308_page;

	this->default_page_map_[400] = webserv_error_400_page;
	this->default_page_map_[401] = webserv_error_401_page;
	this->default_page_map_[402] = webserv_error_402_page;
	this->default_page_map_[403] = webserv_error_403_page;
	this->default_page_map_[404] = webserv_error_404_page;
	this->default_page_map_[405] = webserv_error_405_page;
	this->default_page_map_[406] = webserv_error_406_page;
	this->default_page_map_[407] = "";
	this->default_page_map_[408] = webserv_error_408_page;
	this->default_page_map_[409] = webserv_error_409_page;
	this->default_page_map_[410] = webserv_error_410_page;
	this->default_page_map_[411] = webserv_error_411_page;
	this->default_page_map_[412] = webserv_error_412_page;
	this->default_page_map_[413] = webserv_error_413_page;
	this->default_page_map_[414] = webserv_error_414_page;
	this->default_page_map_[415] = webserv_error_415_page;
	this->default_page_map_[416] = webserv_error_416_page;
	this->default_page_map_[417] = "";
	this->default_page_map_[418] = "";
	this->default_page_map_[419] = "";
	this->default_page_map_[420] = "";
	this->default_page_map_[421] = webserv_error_421_page;
	this->default_page_map_[422] = "";
	this->default_page_map_[423] = "";
	this->default_page_map_[424] = "";
	this->default_page_map_[425] = "";
	this->default_page_map_[426] = "";
	this->default_page_map_[427] = "";
	this->default_page_map_[428] = "";
	this->default_page_map_[429] = webserv_error_429_page;

	this->default_page_map_[494] = webserv_error_494_page;
	this->default_page_map_[495] = webserv_error_495_page;
	this->default_page_map_[496] = webserv_error_496_page;
	this->default_page_map_[497] = webserv_error_497_page;
	this->default_page_map_[498] = webserv_error_404_page;
	this->default_page_map_[499] = "";

	this->default_page_map_[500] = webserv_error_500_page;
	this->default_page_map_[501] = webserv_error_501_page;
	this->default_page_map_[502] = webserv_error_502_page;
	this->default_page_map_[503] = webserv_error_503_page;
	this->default_page_map_[504] = webserv_error_504_page;
	this->default_page_map_[505] = webserv_error_505_page;
	this->default_page_map_[506] = "";
	this->default_page_map_[507] = webserv_error_507_page;
}

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

// parseが失敗したときはその情報どう受け取るか
// uriがファイルでもdenyの影響受ける
bool	ConfigHandler::allowRequest( const config::Server& server, const config::Location* location, const HttpRequest& request, const int cli_sock ) const
{
	struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
	if (getsockname(cli_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addrlen) != 0)
	{
		std::cerr << "webserv: [emerge] getsockname() \"" << cli_sock << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
	}
	//std::cout << "" << client_addr.sin_addr.s_addr << std::endl;

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
		if (location->limit_except.excepted_methods.find(request.method) == location->limit_except.excepted_methods.end())
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

	const std::string&	ConfigHandler::searchFile( const struct TiedServer& tied_servers, const HttpRequest& request, const int cli_sock ) const
{
	unsigned int	status_code;

	// parseが失敗していれば、400 Bad Request
	if (request.parseState == HttpRequest::PARSE_ERROR)
		return searchErrorPage(400);

	const config::Server&	server = searchServerConfig(tied_servers, request.headers.find("Host")->second);
	// server の return を見に行く。今はreturn はlocationにしかない

	const config::Location*	location = searchLongestMatchLocationConfig(server, request.uri);
	// location の　return を見に行く。

	// allowReuestがfalseなら403 Forbidden
	if (!allowRequest(server, location, request, cli_sock))
		return searchErrorPage(403);


	// if (unfinished slash directory)
		// return searchErrorPage(301); // 301 Moved Permanently
	if (FileUtils::isDirectory(server.root.getPath() + request.uri))
		return searchErrorPage(301);

	/* ~ try_filesとindex/autoindexのファイル検索 ~
	 * try_filesはlocationのuriを探すファイルのルートにいれずに内部リダイレクト
	 * index/autoindex はrequestのuriにindexのファイル名を足して探す
	 * 3つともなかったら上位のcontextで検索する
	 */

	// try_filesとindex/autoindexをuriが属するcontextから探して返す。見つからなければ403エラー

	// request uriがそもそもrootディレクティブになければ404 Not Found
}

void	ConfigHandler::writeAcsLog( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	//config::Server&	server = searchServerConfig(server_config, server_name);
	//config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	// access_logがどのコンテキストにあるか
	//
	// access_logのパスすべてに
}
void	ConfigHandler::writeErrLog( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	//config::Server&	server = searchServerConfig(server_config, server_name);
	//config::Location*	location = searchLongestMatchLocationConfig(server, uri);
}

const config::Time&	ConfigHandler::getKeepaliveTimeout( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location*	location = searchLongestMatchLocationConfig(server, uri);
}

const config::Time&	ConfigHandler::getSendTimeout( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location*	location = searchLongestMatchLocationConfig(server, uri);
}

const config::Time&	ConfigHandler::getUseridExpires( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	config::Server&	server = searchServerConfig(server_config, server_name);
	config::Location*	location = searchLongestMatchLocationConfig(server, uri);
}

const config::Server&	ConfigHandler::searchServerConfig( const struct TiedServer& tied_servers, const std::string& server_name ) const
{
	config::Server	*default_server = &tied_servers.servers_[0];

	for (size_t i = 0; i < tied_servers.servers_.size(); i++)
	{
		if (tied_servers.servers_[i]->server_name.getName().find(server_name) != tied_servers.servers_[i]->server_name.getName().end())
			return *tied_servers.servers_[i];
		// default_server特定できるようにする
		if (tied_servers.servers_[i]->listen_list[0].getIsDefaultServer())
			default_server = &tied_servers.servers_[i];
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

const std::string&	ConfigHandler::searchErrorPage( const config::Server& server, const config::Location& location, const unsigned int code )
{
}
