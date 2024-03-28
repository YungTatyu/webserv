#include "ConfigHandler.hpp"
#include "Utils.hpp"
#include "NetworkIOHandler.hpp"
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <iomanip>
#include <ctime>

const static std::string	kACCESS_FD = "access_fd";
const static std::string	 kERROR_FD = "error_fd";
const static std::string	kKEEPALIVE_TIMEOUT = "keepalive_timeout";
const static std::string	kSEND_TIMEOUT = "send_timeout";
const static std::string	kUSERID_EXPIRES = "userid_expires";
const static std::string	kDENY = "deny";
const static std::string	kALIAS = "alias";
const static std::string	kAUTOINDEX = "autoindex";
const static std::string	kROOT = "root";
const static std::string	kERROR_PAGE = "error_page";
const static std::string	kLimitExcept = "limit_except";

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

	return htonl(result);
}

bool	ConfigHandler::addressInLimit( const std::string& ip_addr_str, const uint32_t cli_addr ) const
{
	if (ip_addr_str == "all")
		return true;
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
	else
		return config::DELETE;
}

int	ConfigHandler::allowRequest( const config::Server& server, const config::Location* location, const HttpRequest& request, struct sockaddr_in client_addr ) const
{
	// ------ access の制限 ------
	// configからアドレス制限ディレクトリのあるcontext探す
	if (location && location->directives_set.find(kDENY) != location->directives_set.end())
	{
		if (!limitLoop(location->allow_deny_list, client_addr.sin_addr.s_addr))
			return ACCESS_DENY;
	}
	else if (server.directives_set.find(kDENY) != server.directives_set.end())
	{
		if (!limitLoop(server.allow_deny_list, client_addr.sin_addr.s_addr))
			return ACCESS_DENY;
	}
	else if (this->config_->http.directives_set.find(kDENY) != this->config_->http.directives_set.end())
	{
		if (!limitLoop(this->config_->http.allow_deny_list, client_addr.sin_addr.s_addr))
			return ACCESS_DENY;
	}


	// ------ method の制限 ------
	// location内にlimit_except contextあるか？
	if (location && location->directives_set.find(kLimitExcept) != location->directives_set.end())
	{
		// 制限されたメソッドでなければ、スルー
		// HttpRequestでLIMIT_EXCEPTのenum使ってほしい
		if (location->limit_except.excepted_methods.find(convertRequestMethod(request.method)) == location->limit_except.excepted_methods.end())
		{
			if (!limitLoop(location->limit_except.allow_deny_list, client_addr.sin_addr.s_addr))
				return METHOD_DENY;
		}
	}

	// 問題なければtrue
	return ACCESS_ALLOW;
}

// 最終的なlocationで記録
void	ConfigHandler::writeAcsLog( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);
	writeAccessLog(server, location, msg);
}

void	ConfigHandler::writeAccessLog( const config::Server& server, const config::Location* location, const std::string& msg ) const
{
	// access_logがどのコンテキスがあれば出力する
	if (location && location->directives_set.find(kACCESS_FD) != location->directives_set.end())
	{
		for (size_t i = 0; i < location->access_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(location->access_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
	else if (server.directives_set.find(kACCESS_FD) != server.directives_set.end())
	{
		for (size_t i = 0; i < server.access_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(server.access_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
	else if (this->config_->http.directives_set.find(kACCESS_FD) != this->config_->http.directives_set.end())
	{
		for (size_t i = 0; i < this->config_->http.access_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(this->config_->http.access_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
}

void	ConfigHandler::writeErrLog( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri, const std::string& msg ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);
	writeErrorLog(server, location, msg);
}

void	ConfigHandler::writeErrorLog( const config::Server& server, const config::Location* location, const std::string& msg ) const
{
	if (location && location->directives_set.find(kERROR_FD) != location->directives_set.end())
	{
		for (size_t i = 0; i < location->error_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(location->error_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
	else if (server.directives_set.find(kERROR_FD) != server.directives_set.end())
	{
		for (size_t i = 0; i < server.error_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(server.error_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
	else if (this->config_->http.directives_set.find(kERROR_FD) != this->config_->http.directives_set.end())
	{
		for (size_t i = 0; i < this->config_->http.error_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(this->config_->http.error_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
	else if (this->config_->directives_set.find(kERROR_FD) != this->config_->directives_set.end())
	{
		for (size_t i = 0; i < this->config_->error_fd_list.size(); i++)
		{
			if (Utils::wrapperWrite(this->config_->error_fd_list[i], msg) == -1)
				std::cerr << "webserv: [error] write() failed (" << errno << ": " << std::strerror(errno) << ")" << std::endl;
		}
	}
}

const config::Time&	ConfigHandler::searchKeepaliveTimeout( const struct TiedServer& tied_servers, const std::string& server_name, const std::string& uri ) const
{
	const config::Server&	server = searchServerConfig(tied_servers, server_name);
	const config::Location*	location = searchLongestMatchLocationConfig(server, uri);

	if (location && location->directives_set.find(kKEEPALIVE_TIMEOUT) != location->directives_set.end())
	{
		return location->keepalive_timeout.getTime();
	}
	else if (server.directives_set.find(kKEEPALIVE_TIMEOUT) != server.directives_set.end())
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

	if (location && location->directives_set.find(kSEND_TIMEOUT) != location->directives_set.end())
	{
		return location->send_timeout.getTime();
	}
	else if (server.directives_set.find(kSEND_TIMEOUT) != server.directives_set.end())
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

	if (location && location->directives_set.find(kUSERID_EXPIRES) != location->directives_set.end())
	{
		return location->userid_expires.getTime();
	}
	else if (server.directives_set.find(kUSERID_EXPIRES) != server.directives_set.end())
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

	for (size_t si = 0; si < tied_servers.servers_.size(); si++)
	{
		if (tied_servers.servers_[si]->server_name.getName().find(server_name) != tied_servers.servers_[si]->server_name.getName().end())
			return *tied_servers.servers_[si];
		// default_server特定できるようにする
		for (size_t li = 0; li < tied_servers.servers_[si]->listen_list.size(); li++)
		{
			const config::Listen& tmp_listen = tied_servers.servers_[si]->listen_list[li];
			if (tmp_listen.getIsDefaultServer() &&
				tied_servers.port_ == tmp_listen.getport() &&
				tied_servers.addr_ == tmp_listen.getAddress())
			{
				default_server = tied_servers.servers_[si];
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
	if (location && location->directives_set.find(kERROR_PAGE) != location->directives_set.end())
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
	else if (server.directives_set.find(kERROR_PAGE) != server.directives_set.end())
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
	else if (this->config_->http.directives_set.find(kERROR_PAGE) != this->config_->http.directives_set.end())
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

const struct TiedServer	ConfigHandler::createTiedServer( const std::string addr, const unsigned int port ) const
{
	struct TiedServer tied_server(addr, port);

	for (size_t i = 0; i < config_->http.server_list.size(); i++)
	{
		for (size_t j = 0; j < config_->http.server_list[i].listen_list.size(); j++)
		{
			if (config_->http.server_list[i].listen_list[j].getAddress() == addr &&
				config_->http.server_list[i].listen_list[j].getport() == port)
			{
				tied_server.servers_.push_back(&config_->http.server_list[i]);
			}
		}
	}
	return tied_server;
}

std::string	ConfigHandler::searchRootPath( const config::Server& server, const config::Location* location ) const
{
	if (location)
	{
		if (location->directives_set.find(kROOT) != location->directives_set.end())
			return location->root.getPath();
		if (location->directives_set.find(kALIAS) != location->directives_set.end())
			return location->alias.getPath();
	}
	if (server.directives_set.find(kROOT) != server.directives_set.end())
		return server.root.getPath();
	else if (config_->http.directives_set.find(kROOT) != config_->http.directives_set.end())
		return config_->http.root.getPath();
	// これいるか？上のやつと一緒でいいのでは？
	return config::Root::kDefaultPath_;
}


bool	ConfigHandler::isAutoIndexOn( const config::Server& server, const config::Location* location ) const
{
	if (location && location->directives_set.find(kAUTOINDEX) != location->directives_set.end() && location->autoindex.getIsAutoindexOn())
			return true;
	else if (server.directives_set.find(kAUTOINDEX) != server.directives_set.end() && server.autoindex.getIsAutoindexOn())
			return true;
	else if (config_->http.directives_set.find(kAUTOINDEX) != config_->http.directives_set.end() && config_->http.autoindex.getIsAutoindexOn())
			return true;
	return false;
}

const std::string	getCurrentTimeLogFormat()
{
	std::time_t	currentTime = std::time(NULL);
	std::tm	*gmTime = std::gmtime(&currentTime);

	const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	std::ostringstream	oss;
	oss << days[gmTime->tm_wday] << "/"
		<< months[gmTime->tm_mon] << "/"
		<< 1900 + gmTime->tm_year << ":"
		<< std::setfill('0') << std::setw(2) << gmTime->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << gmTime->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << gmTime->tm_sec
		<< " GMT";

	return oss.str();
}

const std::string	ConfigHandler::createAcsLogMsg( const uint32_t ip, const long status, const HttpRequest& request ) const
{
	std::stringstream	ss;

	std::string	requestMethod, requestUrl, userAgent;
/*
	switch (request.method) {
		case config::LimitExcept::GET:
			requestMethod = "GET";
			break;
		case config::LimitExcept::POST:
			requestMethod = "POST";
			break;
		case config::LimitExcept::DELETE:
			requestMethod = "DELETE";
			break;
		case config::LimitExcept::HEAD:
			requestMethod = "HEAD";
			break;
		default:
			requestMethod = "NONE";
			break;
	}
*/
	// HttpRequest mergeしたら変更
	requestMethod = request.method;
	// URLの表示をするかどうか？
	requestUrl = "-";
	std::map<std::string, std::string>::const_iterator	it = request.headers.find("User-Agent");
	if (it != request.headers.end())
		userAgent = it->second;
	else
		userAgent = "-";

	ss << Utils::ipToStr(ip) << " - - [" << getCurrentTimeLogFormat() << "] \"" << request.method << " " << request.uri << " HTTP/1.1\" " << status << " \"" << requestUrl << "\" \"" << userAgent << "\"" << std::endl;

	return ss.str();
}
