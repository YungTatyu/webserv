#include "Parser.hpp"
#include <iostream>
#include <utility>
#include <sstream>
#include <climits>
#include <algorithm>

std::map<std::string, unsigned int>	config::Parser::all_directives_;

const unsigned int	config::Http::type;
const unsigned int	config::Events::type;
const unsigned int	config::Server::type;
const unsigned int	config::Location::type;
const unsigned int	config::LimitExcept::type;
const unsigned int	config::AccessLog::kType_;
const unsigned int	config::Alias::kType_;
const unsigned int	config::Allow::kType_;
const unsigned int	config::Autoindex::kType_;
const unsigned int	config::ClientMaxBodySize::kType_;
const unsigned int	config::Deny::kType_;
const unsigned int	config::ErrorLog::kType_;
const unsigned int	config::ErrorPage::kType_;
const unsigned int	config::Index::kType_;
const unsigned int	config::KeepaliveTimeout::kType_;
const unsigned int	config::Listen::kType_;
const unsigned int	config::Return::kType_;
const unsigned int	config::Root::kType_;
const unsigned int	config::SendTimeout::kType_;
const unsigned int	config::ServerName::kType_;
const unsigned int	config::TryFiles::kType_;
const unsigned int	config::Use::kType_;
const unsigned int	config::Userid::kType_;
const unsigned int	config::UseridDomain::kType_;
const unsigned int	config::UseridExpires::kType_;
const unsigned int	config::UseridPath::kType_;
const unsigned int	config::UseridService::kType_;
const unsigned int	config::WorkerConnections::kType_;

const static	std::string kHTTP = "http";
const static	std::string kSERVER = "server";
const static	std::string kEVENTS = "events";

config::Parser::Parser(const std::vector<Token> &tokens, const std::string &filepath) :
	tokens_(tokens), filepath_(filepath), ti(0)
{
	// 現在のcontextをセット
	this->current_context_.push(CONF_MAIN);

	// context
	this->all_directives_.insert(std::make_pair("main", CONF_MAIN));
	this->all_directives_.insert(std::make_pair(kEVENTS, config::Events::type));
	this->all_directives_.insert(std::make_pair(kHTTP, config::Http::type));
	this->all_directives_.insert(std::make_pair(kSERVER, config::Server::type));
	this->all_directives_.insert(std::make_pair("location", config::Location::type));
	this->all_directives_.insert(std::make_pair("limit_except", config::LimitExcept::type));

	// directive
	this->all_directives_.insert(std::make_pair("access_log", config::AccessLog::kType_));
	this->all_directives_.insert(std::make_pair("alias", config::Alias::kType_));
	this->all_directives_.insert(std::make_pair("allow", config::Allow::kType_));
	this->all_directives_.insert(std::make_pair("autoindex", config::Autoindex::kType_));
	this->all_directives_.insert(std::make_pair("client_max_body_size", config::ClientMaxBodySize::kType_));
	this->all_directives_.insert(std::make_pair("deny", config::Deny::kType_));
	this->all_directives_.insert(std::make_pair("error_log", config::ErrorLog::kType_));
	this->all_directives_.insert(std::make_pair("error_page", config::ErrorPage::kType_));
	this->all_directives_.insert(std::make_pair("index", config::Index::kType_));
	this->all_directives_.insert(std::make_pair("keepalive_timeout", config::KeepaliveTimeout::kType_));
	this->all_directives_.insert(std::make_pair("listen", config::Listen::kType_));
	this->all_directives_.insert(std::make_pair("return", config::Return::kType_));
	this->all_directives_.insert(std::make_pair("root", config::Root::kType_));
	this->all_directives_.insert(std::make_pair("send_timeout", config::SendTimeout::kType_));
	this->all_directives_.insert(std::make_pair("server_name", config::ServerName::kType_));
	this->all_directives_.insert(std::make_pair("try_files", config::TryFiles::kType_));
	this->all_directives_.insert(std::make_pair("use", config::Use::kType_));
	this->all_directives_.insert(std::make_pair("userid", config::Userid::kType_));
	this->all_directives_.insert(std::make_pair("userid_domain", config::UseridDomain::kType_));
	this->all_directives_.insert(std::make_pair("userid_expires", config::UseridExpires::kType_));
	this->all_directives_.insert(std::make_pair("userid_path", config::UseridPath::kType_));
	this->all_directives_.insert(std::make_pair("userid_service", config::UseridService::kType_));
	this->all_directives_.insert(std::make_pair("worker_connections", config::WorkerConnections::kType_));

	// parser
	this->parser_map_[kHTTP] = &config::Parser::parseHttpServerEvents;
	this->parser_map_[kEVENTS] = &config::Parser::parseHttpServerEvents;
	this->parser_map_[kSERVER] = &config::Parser::parseHttpServerEvents;
	this->parser_map_["location"] = &config::Parser::parseLocation;
	this->parser_map_["limit_except"] = &config::Parser::parseLimitExcept;

	this->parser_map_["access_log"] = &config::Parser::parseAccessLog;
	this->parser_map_["error_log"] = &config::Parser::parseErrorLog;
	this->parser_map_["use"] = &config::Parser::parseUse;
	this->parser_map_["woker_connections"] = &config::Parser::parseWorkerConnections;
	this->parser_map_["send_timeout"] = &config::Parser::parseSendTimeout;
	this->parser_map_["keepalive_timeout"] = &config::Parser::parseKeepaliveTimeout;
	this->parser_map_["client_max_body_size"] = &config::Parser::parseClientMaxBodySize;
	this->parser_map_["root"] = &config::Parser::parseRoot;
	this->parser_map_["index"] = &config::Parser::parseIndex;
	this->parser_map_["autoindex"] = &config::Parser::parseAutoindex;
	this->parser_map_["error_page"] = &config::Parser::parseErrorPage;
	this->parser_map_["allow"] = &config::Parser::parseAllow;
	this->parser_map_["deny"] = &config::Parser::parseDeny;
	this->parser_map_["listen"] = &config::Parser::parseListen;
	this->parser_map_["server_name"] = &config::Parser::parseServerName;
	this->parser_map_["try_files"] = &config::Parser::parseTryFiles;
	this->parser_map_["alias"] = &config::Parser::parseAlias;
	this->parser_map_["return"] = &config::Parser::parseReturn;
	this->parser_map_["userid"] = &config::Parser::parseUserid;
	this->parser_map_["userid_domain"] = &config::Parser::parseUseridDomain;
	this->parser_map_["userid_expires"] = &config::Parser::parseUseridExpires;
	this->parser_map_["userid_path"] = &config::Parser::parseUseridPath;
	this->parser_map_["userid_service"] = &config::Parser::parseUseridService;
}

config::Parser::~Parser() {}

/**
 * parseの流れ
 * 1. directiveが終了しているか: ";", "{"
 * 2. 存在するdirectiveか
 * 3. contextが正しいか
 * 4. argsの数が正しいか
 * 5. 重複を確認
 * 6. argsの値
*/
bool	config::Parser::parse()
{
	while (1)
	{
		const Token &current_token = this->tokens_[ti];
		if (current_token.type_ == TK_END)
			break;

		// "}" tokenの場合、tokenを進める
		if (current_token.type_ == TK_CLOSE_CURLY_BRACE)
		{
			// main contextでは "}" はエラー
			if (this->current_context_.top() == CONF_MAIN)
			{
				printError(std::string("unexpected \"") + current_token.value_ + "\"", current_token);
				return false;
			}
			this->current_context_.pop();
			++ti;
			continue;
		}

		// directiveが終了しているか
		if (!expectTerminatingToken())
			return false;
		
		// 存在するcontextまたはdirectiveか
		if (!isDirective(current_token) && !isContext(current_token))
		{
			printError(std::string("unknown directive ") + "\"" + current_token.value_ + "\"", current_token);
			return false;
		}

		// directiveのtypeを確認
		if (!parseType(current_token))
			return false;

		// directiveのargsの値を確認
		bool (config::Parser::*directive_parser)() = this->parser_map_[current_token.value_];
		if (!(this->*directive_parser)())
			return false;

		// parseされたdirectiveを管理
		this->set_directives_.insert(current_token.value_);
	}

	// current contextがmainでないとerror
	if (this->current_context_.top() != CONF_MAIN)
	{
		printError("unexpected end of file, expecting \"}\"", this->tokens_[ti]);
		return false;
	}
	// events contextが設定されていないとerror
	if (this->set_directives_.find(kEVENTS) == this->set_directives_.end())
	{
		std::cerr << "webserv: [emerg] no \"events\" section in configuration\n";
		return false;
	}
	return true;
}

/**
 * 1. contextが正しいか確認
 * 2. argsの数が正しいか確認
 * 3. 重複を確認
*/
bool	config::Parser::parseType(const Token &token)
{
	const std::string directive_name = token.value_;
	// contextが正しいか
	if (!(this->all_directives_[directive_name] & this->current_context_.top()))
	{
		printError(std::string("\"") + directive_name + "\" directive is not allowed here", token);
		return false;
	}

	// argsの数が正しいか
	const TK_TYPE terminating_token = isContext(token) ? TK_OPEN_CURLY_BRACE : TK_SEMICOLON;
	const size_t	args_num = countArgs(terminating_token);
	bool	ret;
	switch (args_num)
	{
	case 0:
		ret = expectArgsNum(CONF_NOARGS, this->all_directives_[directive_name]);
		break;
	case 1:
		ret = expectArgsNum(CONF_TAKE1|CONF_1MORE, this->all_directives_[directive_name]);
		break;
	case 2:
		ret = expectArgsNum(CONF_TAKE2|CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 3:
		ret = expectArgsNum(CONF_TAKE3|CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 4:
		ret = expectArgsNum(CONF_TAKE4|CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 5:
		ret = expectArgsNum(CONF_TAKE5|CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 6:
		ret = expectArgsNum(CONF_TAKE6|CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 7:
		ret = expectArgsNum(CONF_TAKE7|CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 8:
		ret = expectArgsNum(CONF_1MORE|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	default:
		ret = false;
		break;
	}
	if (!ret)
	{
		printError(std::string("invalid number of arguments in \"") + directive_name + "\" directive", token);
		return false;
	}

	// 重複を確認
	const std::set<std::string>	*directives_set = searchDirectivesSet(this->current_context_.top());
	// directiveが重複不可かつ重複していたらエラー
	if (directives_set != NULL &&
		(this->all_directives_[directive_name] & CONF_UNIQUE) &&
		directives_set->find(directive_name) != directives_set->end()
	)
	{
		printError(std::string("\"" + token.value_ + "\" directive is duplicate"), token);
		return false;
	}
	return true;
}

const std::set<std::string>	*config::Parser::searchDirectivesSet(const CONTEXT context) const
{
	const std::set<std::string>	*ret = NULL;
	switch (context)
	{
	case CONF_MAIN:
		ret = &(this->config_.set_directives);
		break;

	case CONF_HTTP:
		ret = &(this->config_.http.set_directives);
		break;

	case CONF_EVENTS:
		ret = &(this->config_.events.set_directives);
		break;

	case CONF_HTTP_SERVER:
		{
			const std::vector<Server>	&server_list = this->config_.http.server_list;
			// serverがすでに存在している場合は、一番最後にparseしたserverのset_directiveを取得
			ret = server_list.size() != 0 ? &(server_list.back().set_directives) : NULL;
		}
		break;

	case CONF_HTTP_LOCATION:
		{
			const Server	&current_server = this->config_.http.server_list.back();
			const std::vector<Location>	&location_list = current_server.location_list;
			// locationがすでに存在している場合は、一番最後にparseしたlocationのset_directiveを取得
			ret = location_list.size() != 0 ? &(location_list.back().set_directives) : NULL;
		}
		break;

	case CONF_HTTP_LIMIT_EXCEPT:
		{
			const Location	&current_location = this->config_.http.server_list.back().location_list.back();
			ret = &(current_location.limit_except.set_directives);
		}
		break;
	
	default:
		ret = NULL;
		break;
	}
	return ret;
}

bool	config::Parser::expectTokenType(const config::TK_TYPE type, const Token &token) const
{
	if (type != token.type_)
	{
		printError(std::string("unexpected ") + "\"" + token.value_ + "\"", token);
		return false;
	}
	return true;
}

bool	config::Parser::expectArgsNum(const unsigned int expect, const unsigned int actual) const
{
	return expect & actual;
}

/**
 * directiveが ";"もしくは"{"で区切られているか確認
*/
bool	config::Parser::expectTerminatingToken() const
{
	size_t	i = this->ti + 1;

	while (this->tokens_[i].type_ != TK_SEMICOLON && this->tokens_[i].type_ != TK_OPEN_CURLY_BRACE)
	{
		if (this->tokens_[i].type_ == TK_END)
		{
			printError("unexpected end of file, expecting \";\" or \"}\"", this->tokens_[i]);
			return false;
		}
		if (!expectTokenType(TK_STR, this->tokens_[i]))
			return false;
		++i;
	}
	return true;
}

/**
 * main contextはconfで設定されないため、含めない
*/
bool	config::Parser::isContext(const config::Token &token) const
{
	return token.type_ == config::TK_STR &&
	(
		token.value_ == kEVENTS
		|| token.value_ == kHTTP
		|| token.value_ == kSERVER
		|| token.value_ == "location"
		|| token.value_ == "limit_except"
	);
}

bool	config::Parser::isDirective(const config::Token &token) const
{
	return 
	(
		token.type_ == config::TK_STR
		&& !isContext(token)
		&& this->all_directives_.find(token.value_) != this->all_directives_.end()
	);
}

/**
 * tokenを一時的に進める必要があるため、引数でtokenを渡さない
 * @date terminating_token: directive、contextの終了条件
 * directive ;
 * context   {
 * 
*/
size_t	config::Parser::countArgs(const TK_TYPE terminating_token) const
{
	size_t	i = this->ti + 1;
	size_t	args_num = 0;

	while (this->tokens_[i].type_ != terminating_token)
	{
		++i;
		++args_num;
	}
	return args_num;
}

void	config::Parser::printError(const std::string &err_msg, const Token &token) const
{
	std::cerr << "webserv: [emerg] " << err_msg << " in " + this->filepath_ << ":" << token.line_  << '\n';
}

/**
 * http, server, eventsをparse
*/
bool	config::Parser::parseHttpServerEvents()
{
	const std::vector<Token>	&tokens = this->tokens_;
	const std::string	&context = tokens[ti].value_;

	++ti; // tokenをcontextの引数に進める
	// 新たなserver contextを追加
	if (context == kSERVER)
		this->config_.http.server_list.push_back(Server());

	// current contextをupdate
	if (context == kHTTP)
		this->current_context_.push(CONF_HTTP);
	else if (context == kSERVER)
		this->current_context_.push(CONF_HTTP_SERVER);
	else if (context == kEVENTS)
		this->current_context_.push(CONF_EVENTS);
	
	++ti; // 次のtokenに進める
	return true;
}

bool	config::Parser::parseLocation()
{
	const std::vector<Token>	&tokens = this->tokens_;
	++ti; // tokenをcontextの引数に進める

	// locationのuriが重複していないか確認
	const std::string &uri = tokens[ti].value_;
	std::vector<Location>	&list = this->config_.http.server_list.back().location_list;
	for (std::vector<Location>::iterator it = list.begin(); it != list.end(); ++it)
	{
		if (it->uri_ == uri)
		{
			printError(std::string("duplicate location \"") + tokens[ti].value_ + "\"", tokens[ti]);
			return false;
		}
	}
	list.push_back(Location(uri));

	// current contextをupdate
	this->current_context_.push(CONF_HTTP_LOCATION);

	ti += 2; // "{" を飛ばして、次のtokenへ進む
	return true;
}

bool	config::Parser::parseLimitExcept()
{
	const std::vector<Token>	&tokens = this->tokens_;
	std::vector<REQUEST_METHOD>	&list = this->config_.http.server_list.back().location_list.back().limit_except.excepted_methods_;
	++ti; // tokenをcontextの引数に進める
	do
	{
		const std::string upper_case_method = toUpper(tokens[ti].value_);
		if (upper_case_method != "GET" && upper_case_method != "HEAD" && upper_case_method != "POST" && upper_case_method != "DELETE")
		{
			printError(std::string("invalid method \"" + tokens[ti].value_ + "\""), tokens[ti]);
			return false;
		}
		const REQUEST_METHOD	method = convertToRequestMethod(upper_case_method);
		// すでに追加されているmethodならば、新たに追加しない
		if (std::find(list.begin(), list.end(), method) == list.end())
		{
			list.push_back(REQUEST_METHOD(method));
		}
		++ti;
	} while (tokens[ti].type_ != TK_OPEN_CURLY_BRACE);

	// current contextをupdate
	this->current_context_.push(CONF_EVENTS);	

	++ti;
	return true;
}

config::REQUEST_METHOD	config::Parser::convertToRequestMethod(const std::string &method) const
{
	REQUEST_METHOD ret = GET;

	if (method == "GET")
		ret = GET;
	else if (method == "HEAD")
		ret = HEAD;
	else if (method == "POST")
		ret = POST;
	else if (method == "DELETE")
		ret = DELETE;
	return ret;
}

std::string	config::Parser::toUpper(std::string str) const
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		*it = std::toupper(*it);
	}
	return str;
}

bool	config::Parser::parseAccessLog()
{
	ti++;
	std::string	path = this->tokens_[ti].value_;
	config::CONTEXT context = this->current_context_.top();
	config::AccessLog	tmp_acs_log;

	// 文字列が空でなければオブジェクトを追加する
	if (!path.empty())
	{
		tmp_acs_log.setFile(path);

		if (context == config::CONF_HTTP)
			this->config_.http.access_log_list.push_back(tmp_acs_log);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().access_log_list.push_back(tmp_acs_log);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().access_log_list.push_back(tmp_acs_log);
	}

	ti += 2;
	return true;
}

bool	config::Parser::parseErrorLog()
{
	ti++;
	std::string	path = this->tokens_[ti].value_;
	config::CONTEXT context = this->current_context_.top();
	config::ErrorLog	tmp_err_log;

	// 文字列が空でなければオブジェクトを追加する
	if (!path.empty())
	{
		tmp_err_log.setFile(path);

		if (context == config::CONF_MAIN)
			this->config_.error_log_list.push_back(tmp_err_log);
		else if (context == config::CONF_HTTP)
			this->config_.http.error_log_list.push_back(tmp_err_log);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().error_log_list.push_back(tmp_err_log);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().error_log_list.push_back(tmp_err_log);
	}

	ti += 2;
	return true;
}

#if defined(__APPLE__)
const config::OS  currentOS = config::Mac;
#elif defined(__linux__)
const config::OS  currentOS = config::Linux;
#else
const config::OS  currentOS = config::Unknown;
#endif

bool	config::Parser::parseUse()
{
	ti++;

	std::string token_value = this->tokens_[ti].value_;

	std::cout << currentOS << std::endl;
	switch (currentOS) {
		case config::Mac:
			if (!(token_value == "select" ||
				token_value == "poll" ||
				token_value == "kqueue"))
			{
				std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}
			break;
		case config::Linux:
			if (!(token_value == "select" ||
				token_value == "poll" ||
				token_value == "epoll"))
			{
				std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}
			break;
		case config::Unknown:
			if (!(token_value == "select" ||
				token_value == "poll"))
			{
				std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}
			break;
		default:
			return false;
			break;
	}

	config::CONNECTION_METHOD	method;

	if (token_value == "select")
		method = config::SELECT;
	else if (token_value == "poll")
		method = config::POLL;
	else if (token_value == "kqueue")
		method = config::KQUEUE;
	else if (token_value == "epoll")
		method = config::EPOLL;
	else
	{
		std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	this->config_.events.use.setConnectionMethod(method);

	ti += 2;
	return true;
}

bool	config::Parser::parseWorkerConnections()
{
	long	value;
	char	char_remaining;

	ti++;
	std::istringstream	iss(this->tokens_[ti].value_.c_str());

	// 値が数値でない、またはLONG_MAXに収まらなければエラー
	if (iss >> value)
	{
		if (iss >> char_remaining)
		{
			std::cerr << "webserv: [emerg] invalid number \"" << value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}

		if (0 <= value && value <= 1) // 本当はserver側で弾く
		{
			std::cerr << "webserv: [emerg] \"" << value << "\" worker_connections are not enough for 1 listening sockets" << std::endl;
			return false;
		}
		else if (value < 0 || LONG_LONG_MAX < value)
		{
			std::cerr << "webserv: [emerg] invalid number \"" << value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "webserv: [emerg] invalid number \"" << value << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return (false);
	}

	// エラー判定に引っかからなかったのでセット
	this->config_.events.worker_connections.setWorkerConnections(value);

	ti += 2;
	return true;
}

bool	config::Parser::canConvertMinTime(long &value, const std::string& unit)
{
	if (unit == "" || unit == "s")
	{
		if (config::Time::seconds > std::numeric_limits<long>::max() / value)
			return false;
		value *= config::Time::seconds;
	}
	else if (unit == "m")
	{
		if (config::Time::minutes > std::numeric_limits<long>::max() / value)
			return false;
		value *= config::Time::minutes;
	}
	else if (unit == "h")
	{
		if (config::Time::hours > std::numeric_limits<long>::max() / value)
			return false;
		value *= config::Time::hours;
	}
	else if (unit == "d")
	{
		if (config::Time::days < std::numeric_limits<long>::max() / value)
			return false;
		value *= config::Time::days;
	}
	return true;
}

bool	config::Parser::canConvertMinSize(long &value, const std::string& unit)
{
	if (unit == "k" || unit == "K")
	{
		if (config::Size::kilobytes > std::numeric_limits<long>::max() / value)
			return false;
		value *= config::Size::kilobytes;
	}
	else if (unit == "m" || unit == "M")
	{
		if (config::Size::megabytes > std::numeric_limits<long>::max() / value)
			return false;
		value *= config::Size::megabytes;
	}
	return true;
}

long	config::Parser::parseTime()
{
	long			num;
	std::string			unit; //単位
	std::istringstream	iss(this->tokens_[ti].value_.c_str());

	if (iss >> num)
	{
		if (iss >> unit)
		{
			if (unit != "m" &&
				unit != "s" &&
				unit != "h" &&
				unit != "d")
			{
				return -1;
			}
		}
		else
			unit = "";

		if (num == 0)
			return 0;
		if (num < 0 || !canConvertMinTime(num, unit)) // ms 変更できればOK
		{
			return -1;
		}
	}
	else
		return -1;
	return num;
}

long	config::Parser::parseSize()
{
	long			num;
	std::string			unit;
	std::istringstream	iss(this->tokens_[ti].value_.c_str());

	if (iss >> num)
	{
		if (iss >> unit)
		{
			if (unit != "k" &&
				unit != "K" &&
				unit != "m" &&
				unit != "M")
				return -1;
		}
		else
			unit = "";

		if (num == 0)
			return 0;
		if (num < 0 || !canConvertMinSize(num, unit)) // ms 変更できればOK
			return -1;
	}
	else
		return -1;
	return num;
}

bool	config::Parser::parseSendTimeout()
{
	ti++;

	long ret = parseTime();
	if (ret == -1)
	{
		std::cerr << "webserv: [emerg] \"send_timeout\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	this->config_.http.send_timeout.setTime(ret);
	ti += 2;
	return true;
}

bool	config::Parser::parseKeepaliveTimeout()
{
	ti++;

	long ret = parseTime();
	if (ret == -1)
	{
		std::cerr << "webserv: [emerg] \"keepalive_timeout\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	this->config_.http.keepalive_timeout.setTime(ret);
	ti += 2;
	return true;
}

bool	config::Parser::parseRoot()
{
	ti++;
	std::string	path = this->tokens_[ti].value_;
	config::CONTEXT context = this->current_context_.top();

	if (context == config::CONF_HTTP)
		this->config_.http.root.setPath(path);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().root.setPath(path);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().root.setPath(path);

	ti += 2;
	return true;
}

bool	config::Parser::parseClientMaxBodySize()
{
	ti++;

	long ret = parseTime();
	if (ret == -1)
	{
		std::cerr << "webserv: [emerg] \"client_max_body_size\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	this->config_.http.client_max_body_size.setSize(ret);

	return true;
}

bool	config::Parser::parseIndex()
{
	ti++;
	std::string	file;
	config::CONTEXT context = this->current_context_.top();
	config::Index	tmp_index;

	while (this->tokens_[ti].type_ == config::TK_SEMICOLON)
	{
		file = this->tokens_[ti].value_;

		// 空文字列があればエラー
		if (file.empty())
		{
			std::cerr << "webserv: [emerg] index " << file << "in \"index\" directive is invalid in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}

		tmp_index.setFile(file);

		if (context == config::CONF_HTTP)
			this->config_.http.index_list.push_back(tmp_index);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().index_list.push_back(tmp_index);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().index_list.push_back(tmp_index);

		ti++;
	}

	ti++;
	return true;
}

bool	config::Parser::parseAutoindex()
{
	ti++;
	std::string	tmp_switch = this->tokens_[ti].value_;

	if (tmp_switch != "on" && tmp_switch != "off")
	{
		std::cerr << "webserv: [emerg] invalid value " << tmp_switch << " in \"autoindex\" directive, it must be \"on\" or \"off\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	config::CONTEXT context = this->current_context_.top();
	if (tmp_switch == "on")
	{
		if (context == config::CONF_HTTP)
			this->config_.http.autoindex.setIsAutoindexOn(true);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().autoindex.setIsAutoindexOn(true);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().autoindex.setIsAutoindexOn(true);
	}

	ti += 2;
	return true;
}

long	config::Parser::retCodeIfValid()
{
	std::istringstream	iss(this->tokens_[ti].value_.c_str());
	long	code;
	char	remaining_char;

	if (iss >> code)
	{
		if (iss >> remaining_char)
		{
			std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return -1;
		}
		if (300 <= code && code <= 599)
			return code;
		else
		{
			std::cerr << "webserv: [emerg] value \"" << code << "\" must be between 300 and 599 in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return -1;
		}
	}
	else // LONG_MAX/MINを超えたり、数値ではなければエラー
	{
		std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return -1;
	}
}

long	config::Parser::retErrorPageOptNumIfValid()
{
	std::istringstream	iss(this->tokens_[ti].value_.substr(1));
	long	tmp_code;
	char	remaining_char;

	// responseの値の確認
	if (iss >> tmp_code)
	{
		if (iss >> remaining_char || tmp_code < 0)
		{
			std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return -1;
		}
		return tmp_code;
	}
	else // LONG_MAX/MIN を超えたり、数値ではなければエラー
	{
		std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return -1;
	}
}

bool	config::Parser::parseErrorPage()
{
	ti++;
	config::ErrorPage	tmp_err_pg;
	long	code;
	unsigned int	tmp_ti = ti;

	// uriまではcodeとしてみていく
	while (this->tokens_[ti + 1].type_ == config::TK_SEMICOLON)
	{
		// 最後から二番目の引数が=responseオプションの場合
		if (ti != tmp_ti 
			&& this->tokens_[ti + 1].type_ == config::TK_SEMICOLON 
			&& tokens_[ti].value_[0] == '=')
		{
			long	ret = retErrorPageOptNumIfValid();
			if (ret == -1)
				return false;
			tmp_err_pg.setResponse(ret);
			break ;
		}

		code = retCodeIfValid();
		if (code == -1)
			return false;

		tmp_err_pg.addCode(code);

		ti++;
	}

	tmp_err_pg.setUri(this->tokens_[ti].value_);

	if (this->current_context_.top() == config::CONF_HTTP)
		this->config_.http.error_page_list.push_back(tmp_err_pg);
	else if (this->current_context_.top() == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().error_page_list.push_back(tmp_err_pg);
	else if (this->current_context_.top() == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().error_page_list.push_back(tmp_err_pg);

	return true;
}

bool	config::Parser::isIPv4(const std::string& ipv4)
{
	// 1.文字列が空でないかを確認
	if (ipv4.empty())
	{
		return false;
	}

	// 2. IPv6アドレスとサブネットマスクを分割
	size_t	mask_pos = ipv4.find('/');
	std::string address_part = (mask_pos != std::string::npos) ? ipv4.substr(0, mask_pos) : ipv4;
	std::string mask_part = (mask_pos != std::string::npos) ? ipv4.substr(mask_pos + 1) : "";

	// 3. 文字列がIPv4の基本的な構造に従っているかを確認
	std::istringstream iss(ipv4);
	std::string field;
	std::vector<std::string> fields;

	while (std::getline(iss, field, ':'))
	{
		// 各フィールドが数字であることを確認
		for (int i = 0; field[i] != '\0'; i++)
		{
			if (!std::isdigit(field[i]))
			{
				return false;
			}
		}

		// フィールドを保存
		fields.push_back(field);
	}
	
		// フィールドの数が正しいかを確認
	if (fields.size() != 4)
	{
		return false;
	}

	// 各フィールドが0から255までの値を持っていることを確認
	for (int i = 0; i < 4; i++)
	{
		field = fields[i];
		unsigned int value;
		std::istringstream(field) >> std::hex >> value;
		if (255 < value)
		{
			return false;
		}
	}

	// 4. subnetmaskの値が正しいか確認
	if (!mask_part.empty())
	{
		int	subnet_mask;
		std::istringstream(mask_part) >> subnet_mask;

		if (subnet_mask < 0 || 32 < subnet_mask)
		{
			return false;
		}
	}

	// 全ての条件を満たす場合、IPv4アドレスと見なす
	return true;
}

bool	config::Parser::isIPv6(const std::string& ipv6)
{
	// 1.文字列が空でないかを確認
	if (ipv6.empty())
	{
		return false;
	}

	// 2. IPv6アドレスとサブネットマスクを分割
	size_t	mask_pos = ipv6.find('/');
	std::string address_part = (mask_pos != std::string::npos) ? ipv6.substr(0, mask_pos) : ipv6;
	std::string mask_part = (mask_pos != std::string::npos) ? ipv6.substr(mask_pos + 1) : "";

	// 3. 文字列がIPv6の基本的な構造に従っているかを確認
	std::istringstream iss(ipv6);
	std::string field;
	std::vector<std::string> fields;

	while (std::getline(iss, field, ':'))
	{
		// 各フィールドが16進数であることを確認
		for (int i = 0; field[i] != '\0'; i++)
		{
			if (!std::isxdigit(field[i]))
			{
				return false;
			}
		}

		// フィールドを保存
		fields.push_back(field);
	}
	
		// フィールドの数が正しいかを確認
	if (fields.size() != 8)
	{
		return false;
	}

	// 各フィールドが0からFFFFまでの値を持っていることを確認
	for (int i = 0; i < 8; i++)
	{
		field = fields[i];
		unsigned int value;
		std::istringstream(field) >> std::hex >> value;
		if (value > 0xFFFF)
		{
			return false;
		}
	}

	// 4. subnetmaskの値が正しいか確認
	if (!mask_part.empty())
	{
		int	subnet_mask;
		std::istringstream(mask_part) >> subnet_mask;

		if (subnet_mask < 0 || 128 < subnet_mask)
		{
			return false;
		}
	}

	// 全ての条件を満たす場合、IPv6アドレスと見なす
	return true;
}

bool	config::Parser::parseAllow()
{
	ti++;
	std::string	address = this->tokens_[ti].value_;

	if (!isIPv4(address) && !isIPv6(address))
	{
		std::cerr << "webserv: [emerg] invalid parameter \"" << address << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	config::Allow	tmp_allow;
	tmp_allow.setAddress(this->tokens_[ti].value_);
	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
		this->config_.http.allow_list.push_back(tmp_allow);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().allow_list.push_back(tmp_allow);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().allow_list.push_back(tmp_allow);
	else if (context == config::CONF_HTTP_LIMIT_EXCEPT)
		this->config_.http.server_list.back().location_list.back().limit_except.allow_list.push_back(tmp_allow);

	ti += 2;
	return true;
}

bool	config::Parser::parseDeny()
{
	ti++;
	std::string	address = this->tokens_[ti].value_;

	if (!isIPv4(address) && !isIPv6(address))
	{
		std::cerr << "webserv: [emerg] invalid parameter \"" << address << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	config::Deny	tmp_deny;
	tmp_deny.setAddress(this->tokens_[ti].value_);
	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
		this->config_.http.deny_list.push_back(tmp_deny);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().deny_list.push_back(tmp_deny);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().deny_list.push_back(tmp_deny);
	else if (context == config::CONF_HTTP_LIMIT_EXCEPT)
		this->config_.http.server_list.back().location_list.back().limit_except.deny_list.push_back(tmp_deny);

	ti += 2;
	return true;
}

bool	config::Parser::parseListen()
{
	ti++;
	config::Listen				tmp_listen;
	std::istringstream			iss;
	char						remaining_char;
	long						port;
	std::string					token;
	std::vector<std::string>	tokens;

	// 1. もし空文字列ならエラー
	if (this->tokens_[ti].value_.empty())
	{
			std::cerr << "webserv: [emerg] host not found in \"" << this->tokens_[ti].value_ << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	// 2. ip addressとportに分ける
	while (getline(iss, token, ':'))
	{
		tokens.push_back(token);
	}

	// 3. 2つ以上に分かれてしまっていたらエラー
	if (tokens.size() > 2)
	{
		std::cerr << "webserv: [emerg] invalid parameter \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	// 4. ip address と portがある場合
	if (tokens.size() == 2)
	{
		// ip addressがあれば値を確認する。
		// なければデフォルト値を入れる。
		if (tokens[0].empty())
		{
			std::cerr << "webserv: [emerg] no host in \"" << this->tokens_[ti].value_ << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}
		else if (!isIPv4(tokens[0]) && !isIPv6(tokens[0]))
		{
			std::cerr << "webserv: [emerg] host not found in \"" << this->tokens_[ti].value_ << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}
		tmp_listen.setAddress(tokens[0]);

		// port番号があれば値を確認しする。
		// なければデフォルト値を入れる。
		if (tokens[1].empty())
		{
			std::cerr << "webserv: [emerg] invalid port in \"" << this->tokens_[ti].value_ << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}
		else
		{
			iss.str(tokens[1].c_str());
			if (iss >> port)
			{
				if (iss >> remaining_char)
				{
					std::cerr << "webserv: [emerg] host not found in \"" << tokens[1] << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
					return false;
				}
				if (port < 0 || port < 65535)
				{
					std::cerr << "webserv: [emerg] invalid port in \"" << tokens[1] << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
					return false;
				}
			}
			else
			{
				std::cerr << "webserv: [emerg] host not found in \"" << tokens[1] << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}
		}
		tmp_listen.setPort(port);
	}
	else
	{
		// 5. ip addressかportのどちらかしかない場合
		iss.str(tokens[0].c_str());

		// ip addressかportであればセット
		if (isIPv4(tokens[0]) || isIPv6(tokens[0]))
			tmp_listen.setAddress(tokens[0]);
		else if (iss >> port)
		{
			if (!(iss >> remaining_char) 
				&& (0 <= port && port <= 65535))
			{
				tmp_listen.setPort(port);
			}
		}
		else
		{
			// addressでもportでもなければエラー
			std::cerr << "webserv: [emerg] host not found in \"" << tokens[1] << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}
	}

	this->config_.http.server_list.back().listen_list.push_back(tmp_listen);

	ti += 2;
	return true;
}

bool	config::Parser::parseServerName()
{
	ti++;
	config::ServerName	tmp_server_name;

	while (this->tokens_[ti].type_ == config::TK_SEMICOLON)
	{
		tmp_server_name.setName(this->tokens_[ti].value_);

		this->config_.http.server_list.back().server_name_list.push_back(tmp_server_name);
		ti++;
	}

	ti++;
	return true;
}

bool	config::Parser::parseTryFiles()
{
	ti++;
	config::CONTEXT	context = this->current_context_.top();
	std::string		file;

	// 最後から2つ目までのトークンはfileとして追加
	while (this->tokens_[ti + 1].type_ == config::TK_SEMICOLON)
	{
		file = this->tokens_[ti].value_;

		if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().try_files.addFile(file);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().try_files.addFile(file);

		ti++;
	}

	// codeかuriか判定
	std::string			uri;
	long				code;
	std::istringstream	iss;
	char				remaining_char;

	if (this->tokens_[ti].value_[0] == '=')
	{
		// code
		iss.str(this->tokens_[ti].value_.substr(1));

		// code が正しいか判定
		if (iss >> code)
		{
			if (iss >> remaining_char
				|| code < 0
				|| 999 < code)
			{
				std::cerr << "webserv: [emerg] invalid code \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "webserv: [emerg] invalid code \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}

		// setCode
		if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().try_files.setCode(code);
		else
			this->config_.http.server_list.back().location_list.back().try_files.setCode(code);
	}
	else
	{
		// uri
		uri = this->tokens_[ti].value_;

		// setUri
		if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().try_files.setUri(uri);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().try_files.setUri(uri);
	}

	ti += 2;
	return true;
}

bool	config::Parser::parseAlias()
{
	ti++;
	std::string	path = this->tokens_[ti].value_;

	this->config_.http.server_list.back().location_list.back().alias.setPath(path);

	ti += 2;
	return true;
}

bool	config::Parser::parseReturn()
{
	ti++;
	long				code;
	std::string			url;
	std::istringstream	iss;
	char				remaining_char;
	config::Return		tmp_return;

	// もしトークンが2つあるなら一つ目はcodeなので処理する
	if (this->tokens_[ti + 2].type_ == config::TK_SEMICOLON)
	{
		iss.str(this->tokens_[ti].value_.c_str());

		if (iss >> code)
		{
			if (iss >> remaining_char)
			{
				std::cerr << "webserv: [emerg] invalid return code \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}

			if (code < 0 || 999 < code)
			{
				std::cerr << "webserv: [emerg] invalid return code \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "webserv: [emerg] invalid return code \"" << this->tokens_[ti].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}

		ti++;
		tmp_return.setCode(code);
	}

	// url をセットする
	url = this->tokens_[ti].value_;
	tmp_return.setUrl(url);

	this->config_.http.server_list.back().location_list.back().return_list.push_back(tmp_return);

	ti += 2;
	return true;
}

bool	config::Parser::parseUserid()
{
	ti++;
	std::string	tmp_switch = this->tokens_[ti].value_;

	// もし、on/offではなかったらエラー
	if (tmp_switch != "on" && tmp_switch != "off")
	{
		std::cerr << "webserv: [emerg] invalid value " << tmp_switch << " in \"userid\" directive, it must be \"on\" or \"off\" in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	// もし、onであれば、
	config::CONTEXT context = this->current_context_.top();
	if (tmp_switch == "on")
	{
		if (context == config::CONF_HTTP)
			this->config_.http.userid.setIsUseridOn(true);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().userid.setIsUseridOn(true);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().userid.setIsUseridOn(true);
	}

	ti += 2;
	return true;
}

bool	config::Parser::parseUseridDomain()
{
	ti++;
	std::string		name = this->tokens_[ti].value_;
	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
		this->config_.http.userid_domain.setName(name);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().userid_domain.setName(name);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().userid_domain.setName(name);

	ti += 2;
	return true;
}

bool	config::Parser::parseUseridExpires()
{
	ti++;
	std::string	tmp_switch = this->tokens_[ti].value_;

	// off であれば、なにもしない
	if (tmp_switch == "off")
		return true;

	long time = parseTime();
	if (time == -1)
	{
		std::cerr << "webserv: [emerg] \"client_max_body_size\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.userid_expires.setTime(time);
		this->config_.http.userid_expires.setIsUseridExpiresOn(true);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().userid_expires.setTime(time);
		this->config_.http.server_list.back().userid_expires.setIsUseridExpiresOn(true);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().userid_expires.setTime(time);
		this->config_.http.server_list.back().location_list.back().userid_expires.setIsUseridExpiresOn(true);
	}

	ti += 2;
	return true;
}

bool	config::Parser::parseUseridPath()
{
	ti++;
	std::string	path = this->tokens_[ti].value_;
	config::CONTEXT	context = this->current_context_.top();

	// もし値が空文字列でなければセットする
	if (!path.empty())
	{
		if (context == config::CONF_HTTP)
			this->config_.http.userid_path.setPath(path);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().userid_path.setPath(path);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().userid_path.setPath(path);
	}

	ti += 2;
	return true;
}

bool	config::Parser::parseUseridService()
{
	ti++;
	long				user_id;
	std::istringstream	iss(this->tokens_[ti].value_.c_str());
	char				remaining_char;

	if (iss >> user_id)
	{
		if (iss >> remaining_char)
		{
			std::cerr << "webserv: [emerg] \"userid_service\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
			return false;
		}

	}
	else
	{
		std::cerr << "webserv: [emerg] \"userid_service\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti].line_ << std::endl;
		return false;
	}

	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
		this->config_.http.userid_service.setUseridService(user_id);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().userid_service.setUseridService(user_id);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().userid_service.setUseridService(user_id);

	ti += 2;
	return true;
}

const config::Main	&config::Parser::getConfig() const
{
	return this->config_;
}

