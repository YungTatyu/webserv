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
const static	std::string kEVENTS = "events";
const static	std::string kSERVER = "server";
const static	std::string kLOCATION = "location";
const static	std::string kLIMIT_EXCEPT = "limit_except";
const static	std::string kACCESS_LOG = "access_log";
const static	std::string kALIAS = "alias";
const static	std::string kALLOW = "allow";
const static	std::string kAUTOINDEX = "autoindex";
const static	std::string kCLIENT_MAX_BODY_SIZE = "client_max_body_size";
const static	std::string kDENY = "deny";
const static	std::string kERROR_LOG = "error_log";
const static	std::string kERROR_PAGE = "error_page";
const static	std::string kINDEX = "index";
const static	std::string kKEEPALIVE_TIMEOUT = "keepalive_timeout";
const static	std::string kLISTEN = "listen";
const static	std::string kRETURN = "return";
const static	std::string kROOT = "root";
const static	std::string kSEND_TIMEOUT = "send_timeout";
const static	std::string kSERVER_NAME = "server_name";
const static	std::string kTRY_FILES = "try_files";
const static	std::string kUSE = "use";
const static	std::string kUSERID = "userid";
const static	std::string kUSERID_DOMAIN = "userid_domain";
const static	std::string kUSERID_EXPIRES = "userid_expires";
const static	std::string kUSERID_PATH = "userid_path";
const static	std::string kUSERID_SERVICE = "userid_service";
const static	std::string kWORKER_CONNECTIONS = "worker_connections";
const static	std::string kSELECT = "select";
const static	std::string kPOLL = "poll";
const static	std::string kEPOLL = "epoll";
const static	std::string kKQUEUE = "kqueue";

config::Parser::Parser(Main &config, const std::vector<Token> &tokens, const std::string &filepath) :
	config_(config), tokens_(tokens), filepath_(filepath), ti_(0)
{
	// 現在のcontextをセット
	this->current_context_.push(CONF_MAIN);

	// context
	this->all_directives_.insert(std::make_pair("main", CONF_MAIN));
	this->all_directives_.insert(std::make_pair(kEVENTS, config::Events::type));
	this->all_directives_.insert(std::make_pair(kHTTP, config::Http::type));
	this->all_directives_.insert(std::make_pair(kSERVER, config::Server::type));
	this->all_directives_.insert(std::make_pair(kLOCATION, config::Location::type));
	this->all_directives_.insert(std::make_pair(kLIMIT_EXCEPT, config::LimitExcept::type));

	// directive
	this->all_directives_.insert(std::make_pair(kACCESS_LOG, config::AccessLog::kType_));
	this->all_directives_.insert(std::make_pair(kALIAS, config::Alias::kType_));
	this->all_directives_.insert(std::make_pair(kALLOW, config::Allow::kType_));
	this->all_directives_.insert(std::make_pair(kAUTOINDEX, config::Autoindex::kType_));
	this->all_directives_.insert(std::make_pair(kCLIENT_MAX_BODY_SIZE, config::ClientMaxBodySize::kType_));
	this->all_directives_.insert(std::make_pair(kDENY, config::Deny::kType_));
	this->all_directives_.insert(std::make_pair(kERROR_LOG, config::ErrorLog::kType_));
	this->all_directives_.insert(std::make_pair(kERROR_PAGE, config::ErrorPage::kType_));
	this->all_directives_.insert(std::make_pair(kINDEX, config::Index::kType_));
	this->all_directives_.insert(std::make_pair(kKEEPALIVE_TIMEOUT, config::KeepaliveTimeout::kType_));
	this->all_directives_.insert(std::make_pair(kLISTEN, config::Listen::kType_));
	this->all_directives_.insert(std::make_pair(kRETURN, config::Return::kType_));
	this->all_directives_.insert(std::make_pair(kROOT, config::Root::kType_));
	this->all_directives_.insert(std::make_pair(kSEND_TIMEOUT, config::SendTimeout::kType_));
	this->all_directives_.insert(std::make_pair(kSERVER_NAME, config::ServerName::kType_));
	this->all_directives_.insert(std::make_pair(kTRY_FILES, config::TryFiles::kType_));
	this->all_directives_.insert(std::make_pair(kUSE, config::Use::kType_));
	this->all_directives_.insert(std::make_pair(kUSERID, config::Userid::kType_));
	this->all_directives_.insert(std::make_pair(kUSERID_DOMAIN, config::UseridDomain::kType_));
	this->all_directives_.insert(std::make_pair(kUSERID_EXPIRES, config::UseridExpires::kType_));
	this->all_directives_.insert(std::make_pair(kUSERID_PATH, config::UseridPath::kType_));
	this->all_directives_.insert(std::make_pair(kUSERID_SERVICE, config::UseridService::kType_));
	this->all_directives_.insert(std::make_pair(kWORKER_CONNECTIONS, config::WorkerConnections::kType_));

	// parser
	this->parser_map_[kHTTP] = &config::Parser::parseHttpServerEvents;
	this->parser_map_[kEVENTS] = &config::Parser::parseHttpServerEvents;
	this->parser_map_[kSERVER] = &config::Parser::parseHttpServerEvents;
	this->parser_map_[kLOCATION] = &config::Parser::parseLocation;
	this->parser_map_[kLIMIT_EXCEPT] = &config::Parser::parseLimitExcept;

	this->parser_map_[kACCESS_LOG] = &config::Parser::parseAccessLog;
	this->parser_map_[kERROR_LOG] = &config::Parser::parseErrorLog;
	this->parser_map_[kUSE] = &config::Parser::parseUse;
	this->parser_map_[kWORKER_CONNECTIONS] = &config::Parser::parseWorkerConnections;
	this->parser_map_[kSEND_TIMEOUT] = &config::Parser::parseSendTimeout;
	this->parser_map_[kKEEPALIVE_TIMEOUT] = &config::Parser::parseKeepaliveTimeout;
	this->parser_map_[kCLIENT_MAX_BODY_SIZE] = &config::Parser::parseClientMaxBodySize;
	this->parser_map_[kROOT] = &config::Parser::parseRoot;
	this->parser_map_[kINDEX] = &config::Parser::parseIndex;
	this->parser_map_[kAUTOINDEX] = &config::Parser::parseAutoindex;
	this->parser_map_[kERROR_PAGE] = &config::Parser::parseErrorPage;
	this->parser_map_[kALLOW] = &config::Parser::parseAllow;
	this->parser_map_[kDENY] = &config::Parser::parseDeny;
	this->parser_map_[kLISTEN] = &config::Parser::parseListen;
	this->parser_map_[kSERVER_NAME] = &config::Parser::parseServerName;
	this->parser_map_[kTRY_FILES] = &config::Parser::parseTryFiles;
	this->parser_map_[kALIAS] = &config::Parser::parseAlias;
	this->parser_map_[kRETURN] = &config::Parser::parseReturn;
	this->parser_map_[kUSERID] = &config::Parser::parseUserid;
	this->parser_map_[kUSERID_DOMAIN] = &config::Parser::parseUseridDomain;
	this->parser_map_[kUSERID_EXPIRES] = &config::Parser::parseUseridExpires;
	this->parser_map_[kUSERID_PATH] = &config::Parser::parseUseridPath;
	this->parser_map_[kUSERID_SERVICE] = &config::Parser::parseUseridService;
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
		const Token &current_token = this->tokens_[ti_];
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
			++ti_;
			continue;
		}

		// ";", "{"が存在するはず
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
	}

	// current contextがmainでないとerror
	if (this->current_context_.top() != CONF_MAIN)
	{
		printError("unexpected end of file, expecting \"}\"", this->tokens_[ti_]);
		return false;
	}
	// events contextが設定されていないとerror
	if (this->config_.directives_set.find(kEVENTS) == this->config_.directives_set.end())
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
	const std::set<std::string>	*directives_set = findDirectivesSet(this->current_context_.top());
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

const std::set<std::string>	*config::Parser::findDirectivesSet(const CONTEXT context) const
{
	const std::set<std::string>	*ret = NULL;
	switch (context)
	{
	case CONF_MAIN:
		ret = &(this->config_.directives_set);
		break;

	case CONF_HTTP:
		ret = &(this->config_.http.directives_set);
		break;

	case CONF_EVENTS:
		ret = &(this->config_.events.directives_set);
		break;

	case CONF_HTTP_SERVER:
		{
			const std::vector<Server>	&server_list = this->config_.http.server_list;
			// serverがすでに存在している場合は、一番最後にparseしたserverのset_directiveを取得
			ret = server_list.size() != 0 ? &(server_list.back().directives_set) : NULL;
		}
		break;

	case CONF_HTTP_LOCATION:
		{
			const Server	&current_server = this->config_.http.server_list.back();
			const std::vector<Location>	&location_list = current_server.location_list;
			// locationがすでに存在している場合は、一番最後にparseしたlocationのset_directiveを取得
			ret = location_list.size() != 0 ? &(location_list.back().directives_set) : NULL;
		}
		break;

	case CONF_HTTP_LIMIT_EXCEPT:
		{
			const LimitExcept	&current_limit_except = this->config_.http.server_list.back().location_list.back().limit_except;
			ret = &(current_limit_except.directives_set);
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

/**
 * expectのbitがactualにも立っているか確認
 * 
 * 以下の場合はtrue:
 * expect: 0100
 * actual: 0111
 * 
 * 以下の場合はfalse
 * expect: 0100
 * actual: 1011
 * 
*/
bool	config::Parser::expectArgsNum(const unsigned int expect, const unsigned int actual) const
{
	return expect & actual;
}

/**
 * directiveが ";"もしくは"{"で区切られているか確認
*/
bool	config::Parser::expectTerminatingToken() const
{
	size_t	i = this->ti_ + 1;

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
		|| token.value_ == kLOCATION
		|| token.value_ == kLIMIT_EXCEPT
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
 * @param terminating_token: directive、contextの終了条件
 * directive ;
 * context   {
 * 
*/
size_t	config::Parser::countArgs(const TK_TYPE terminating_token) const
{
	size_t	i = this->ti_ + 1;
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
	const std::string	&context = tokens[ti_].value_;

	++ti_; // tokenをcontextの引数に進める

	// current contextをupdate
	if (context == kHTTP)
	{
		this->current_context_.push(CONF_HTTP);
		this->config_.directives_set.insert(kHTTP);
	}
	else if (context == kSERVER)
	{
		// 新たなserver contextを追加
		this->config_.http.server_list.push_back(Server());
		this->current_context_.push(CONF_HTTP_SERVER);
		this->config_.http.directives_set.insert(kSERVER);
	}
	else if (context == kEVENTS)
	{
		this->current_context_.push(CONF_EVENTS);
		this->config_.directives_set.insert(kEVENTS);
	}
	
	++ti_; // 次のtokenに進める
	return true;
}

bool	config::Parser::parseLocation()
{
	const std::vector<Token>	&tokens = this->tokens_;
	++ti_; // tokenをcontextの引数に進める

	// locationのuriが重複していないか確認
	const std::string &uri = tokens[ti_].value_;
	std::vector<Location>	&list = this->config_.http.server_list.back().location_list;
	for (std::vector<Location>::iterator it = list.begin(); it != list.end(); ++it)
	{
		if (it->uri == uri)
		{
			printError(std::string("duplicate location \"") + tokens[ti_].value_ + "\"", tokens[ti_]);
			return false;
		}
	}
	list.push_back(Location(uri));

	// current contextをupdate
	this->current_context_.push(CONF_HTTP_LOCATION);

	// serverにlocationをset
	this->config_.http.server_list.back().directives_set.insert(kLOCATION);

	ti_ += 2; // "{" を飛ばして、次のtokenへ進む
	return true;
}

bool	config::Parser::parseLimitExcept()
{
	const std::vector<Token>	&tokens = this->tokens_;
	std::set<REQUEST_METHOD>	&excepted_methods = this->config_.http.server_list.back().location_list.back().limit_except.excepted_methods;
	++ti_; // tokenをcontextの引数に進める
	do
	{
		const std::string upper_case_method = toUpper(tokens[ti_].value_);
		if (upper_case_method != "GET" && upper_case_method != "HEAD" && upper_case_method != "POST" && upper_case_method != "DELETE")
		{
			printError(std::string("invalid method \"" + tokens[ti_].value_ + "\""), tokens[ti_]);
			return false;
		}
		const REQUEST_METHOD	method = convertToRequestMethod(upper_case_method);
		excepted_methods.insert(method);
		++ti_;
	} while (tokens[ti_].type_ != TK_OPEN_CURLY_BRACE);

	// current contextをupdate
	this->current_context_.push(CONF_HTTP_LIMIT_EXCEPT);

	// locationにlimit_exceptをset
	this->config_.http.server_list.back().location_list.back().directives_set.insert(kLIMIT_EXCEPT);

	++ti_;
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
	ti_++;
	std::string	path = this->tokens_[ti_].value_;
	config::CONTEXT context = this->current_context_.top();
	config::AccessLog	tmp_acs_log;

	// 文字列が空でなければオブジェクトを追加する
	if (!path.empty())
	{
		ti_ += 2;
		return true;
	}

	if (context == config::CONF_HTTP)
		this->config_.http.directives_set.insert(kACCESS_LOG);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().directives_set.insert(kACCESS_LOG);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kACCESS_LOG);


	if (path == "off")
		tmp_acs_log.setIsAccesslogOn(false);
	else
		tmp_acs_log.setFile(path);


	if (context == config::CONF_HTTP)
		this->config_.http.access_log_list.push_back(tmp_acs_log);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().access_log_list.push_back(tmp_acs_log);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().access_log_list.push_back(tmp_acs_log);

	ti_ += 2;
	return true;
}

bool	config::Parser::parseErrorLog()
{
	ti_++;
	std::string	path = this->tokens_[ti_].value_;
	config::CONTEXT context = this->current_context_.top();
	config::ErrorLog	tmp_err_log;

	// 文字列が空でなければオブジェクトを追加する
	if (!path.empty())
	{
		ti_ += 2;
		return true;
	}

	tmp_err_log.setFile(path);

	if (context == config::CONF_MAIN)
	{
		this->config_.error_log_list.push_back(tmp_err_log);
		this->config_.directives_set.insert(kERROR_LOG);
	}
	else if (context == config::CONF_HTTP)
	{
		this->config_.http.error_log_list.push_back(tmp_err_log);
		this->config_.http.directives_set.insert(kERROR_LOG);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().error_log_list.push_back(tmp_err_log);
		this->config_.http.server_list.back().directives_set.insert(kERROR_LOG);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().error_log_list.push_back(tmp_err_log);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kERROR_LOG);
	}

	ti_ += 2;
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
	ti_++;

	std::string token_value = this->tokens_[ti_].value_;

	switch (currentOS) {
		case config::Mac:
			if (token_value != kSELECT &&
				token_value != kPOLL &&
				token_value != kKQUEUE)
			{
				std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
			break;
		case config::Linux:
			if (token_value != kSELECT &&
				token_value != kPOLL &&
				token_value != kEPOLL)
			{
				std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
			break;
		case config::Unknown:
			if (token_value != kSELECT &&
				token_value != kPOLL)
			{
				std::cerr << "webserv: [emerg] invalid event type \"" << token_value << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
			break;
		default:
			return false;
			break;
	}

	std::map<std::string, config::CONNECTION_METHOD>	method_map;
	config::CONNECTION_METHOD	method;

	method_map[kSELECT] = config::SELECT;
	method_map[kPOLL] = config::POLL;
	method_map[kEPOLL] = config::EPOLL;
	method_map[kKQUEUE] = config::KQUEUE;
	method = method_map.find(token_value)->second;

	this->config_.events.use.setConnectionMethod(method);
	this->config_.events.directives_set.insert(kUSE);

	ti_ += 2;
	return true;
}

bool	config::Parser::parseWorkerConnections()
{
	ti_++;
	long	value;
	std::string	str = this->tokens_[ti_].value_;


	// 数値でなければエラー
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (!std::isdigit(str[i]))
		{
			std::cerr << "webserv: [emerg] invalid number \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
	}

	std::istringstream	iss(str);

	// 値が正の数かつLONG_MAX以内でなければエラー
	if (iss >> value)
	{
		if (0 == value || value == 1) // 本当はserver側で弾く
		{
			std::cerr << "webserv: [emerg] \"" << value << "\" worker_connections are not enough for 1 listening sockets" << std::endl;
			return false;
		}
		else if (value < 0)
		{
			std::cerr << "webserv: [emerg] invalid number \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "webserv: [emerg] invalid number \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}


	this->config_.events.worker_connections.setWorkerConnections(value);
	this->config_.events.directives_set.insert(kWORKER_CONNECTIONS);

	ti_ += 2;
	return true;
}

bool	config::Parser::canConvertMinTime(long &value, const std::string& unit)
{
	if (unit == "" || unit == "s")
	{
		if (config::Time::seconds > (config::Time::kMaxTimeInMilliseconds_ / value))
			return false;
		value *= config::Time::seconds;
	}
	else if (unit == "m")
	{
		if (config::Time::minutes > (config::Time::kMaxTimeInMilliseconds_ / value))
			return false;
		value *= config::Time::minutes;
	}
	else if (unit == "h")
	{
		if (config::Time::hours > (config::Time::kMaxTimeInMilliseconds_ / value))
			return false;
		value *= config::Time::hours;
	}
	else if (unit == "d")
	{
		if (config::Time::days > (config::Time::kMaxTimeInMilliseconds_ / value))
			return false;
		value *= config::Time::days;
	}
	return true;
}

bool	config::Parser::canConvertMinSize(long &value, const std::string& unit)
{
	if (unit == "k" || unit == "K")
	{
		if (config::Size::kilobytes > (config::Size::kMaxSizeInBytes_ / value))
			return false;
		value *= config::Size::kilobytes;
	}
	else if (unit == "m" || unit == "M")
	{
		if (config::Size::megabytes > (config::Size::kMaxSizeInBytes_ / value))
			return false;
		value *= config::Size::megabytes;
	}
	return true;
}

long	config::Parser::parseTime()
{
	long			num;
	std::string			unit; //単位
	std::istringstream	iss(this->tokens_[ti_].value_.c_str());

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
	std::istringstream	iss(this->tokens_[ti_].value_.c_str());

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
	ti_++;

	long ret = parseTime();
	if (ret == -1)
	{
		std::cerr << "webserv: [emerg] \"send_timeout\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.send_timeout.setTime(ret);
		this->config_.http.directives_set.insert(kSEND_TIMEOUT);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().send_timeout.setTime(ret);
		this->config_.http.server_list.back().directives_set.insert(kSEND_TIMEOUT);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().send_timeout.setTime(ret);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kSEND_TIMEOUT);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseKeepaliveTimeout()
{
	ti_++;

	long ret = parseTime();
	if (ret == -1)
	{
		std::cerr << "webserv: [emerg] \"keepalive_timeout\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	this->config_.http.keepalive_timeout.setTime(ret);
	this->config_.http.directives_set.insert(kKEEPALIVE_TIMEOUT);
	ti_ += 2;
	return true;
}

bool	config::Parser::parseRoot()
{
	ti_++;
	std::string	path = this->tokens_[ti_].value_;
	config::CONTEXT context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.root.setPath(path);
		this->config_.http.directives_set.insert(kROOT);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().root.setPath(path);
		this->config_.http.server_list.back().directives_set.insert(kROOT);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().root.setPath(path);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kROOT);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseClientMaxBodySize()
{
	ti_++;

	long ret = parseSize();
	if (ret == -1)
	{
		std::cerr << "webserv: [emerg] \"client_max_body_size\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	this->config_.http.client_max_body_size.setSize(ret);
	this->config_.http.directives_set.insert(kCLIENT_MAX_BODY_SIZE);

	return true;
}

bool	config::Parser::parseIndex()
{
	ti_++;
	std::string	file;
	config::CONTEXT context = this->current_context_.top();
	config::Index	tmp_index;


	while (this->tokens_[ti_].type_ != config::TK_SEMICOLON)
	{
		file = this->tokens_[ti_].value_;

		// 空文字列があればエラー
		if (file.empty())
		{
			std::cerr << "webserv: [emerg] index " << file << "in \"index\" directive is invalid in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}

		tmp_index.setFile(file);

		if (context == config::CONF_HTTP)
			this->config_.http.index_list.push_back(tmp_index);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().index_list.push_back(tmp_index);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().index_list.push_back(tmp_index);
		ti_++;
	}

	if (context == config::CONF_HTTP)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kINDEX);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kINDEX);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kINDEX);
	ti_++;
	return true;
}

bool	config::Parser::parseAutoindex()
{
	ti_++;
	std::string	tmp_switch = this->tokens_[ti_].value_;

	if (tmp_switch != "on" && tmp_switch != "off")
	{
		std::cerr << "webserv: [emerg] invalid value " << tmp_switch << " in \"autoindex\" directive, it must be \"on\" or \"off\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	config::CONTEXT context = this->current_context_.top();

	if (context == config::CONF_HTTP)
		this->config_.http.directives_set.insert(kAUTOINDEX);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().directives_set.insert(kAUTOINDEX);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kAUTOINDEX);

	if (tmp_switch == "on")
	{
		if (context == config::CONF_HTTP)
			this->config_.http.autoindex.setIsAutoindexOn(true);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().autoindex.setIsAutoindexOn(true);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().autoindex.setIsAutoindexOn(true);
	}

	ti_ += 2;
	return true;
}

unsigned int	config::Parser::retCodeIfValid()
{
	std::istringstream	iss(this->tokens_[ti_].value_.c_str());
	long	code;
	char	remaining_char;

	if (iss >> code)
	{
		if (iss >> remaining_char)
		{
			std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return 0;
		}
		if (300 <= code && code <= 599)
			return static_cast<unsigned int>(code);
		else
		{
			std::cerr << "webserv: [emerg] value \"" << code << "\" must be between 300 and 599 in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return 0;
		}
	}
	else // LONG_MAX/MINを超えたり、数値ではなければエラー
	{
		std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return 0;
	}
}

long	config::Parser::retErrorPageOptNumIfValid()
{
	std::istringstream	iss(this->tokens_[ti_].value_.substr(1));
	long	tmp_code;
	char	remaining_char;

	// responseの値の確認
	if (iss >> tmp_code)
	{
		if (iss >> remaining_char || tmp_code < 0)
		{
			std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return 0;
		}
		return tmp_code;
	}
	else // LONG_MAX/MIN を超えたり、数値ではなければエラー
	{
		std::cerr << "webserv: [emerg] invalid value \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return 0;
	}
}

bool	config::Parser::parseErrorPage()
{
	ti_++;
	config::ErrorPage	tmp_err_pg;
	unsigned int	code;
	unsigned int	tmp_ti = ti_;

	// uriまではcodeとしてみていく
	while (this->tokens_[ti_ + 1].type_ != config::TK_SEMICOLON)
	{
		// 最後から二番目の引数が=responseオプションの場合
		if (ti_ != tmp_ti 
			&& this->tokens_[ti_ + 2].type_ == config::TK_SEMICOLON 
			&& tokens_[ti_].value_[0] == '=')
		{
			long	response = retErrorPageOptNumIfValid();
			if (!response)
				return false;
			tmp_err_pg.setResponse(response);
			ti_++;
			break ;
		}

		code = retCodeIfValid();
		if (!code)
			return false;

		tmp_err_pg.addCode(code);

		ti_++;
	}

	tmp_err_pg.setUri(this->tokens_[ti_].value_);

	if (this->current_context_.top() == config::CONF_HTTP)
	{
		this->config_.http.error_page_list.push_back(tmp_err_pg);
		this->config_.http.directives_set.insert(kERROR_PAGE);
	}
	else if (this->current_context_.top() == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().error_page_list.push_back(tmp_err_pg);
		this->config_.http.server_list.back().directives_set.insert(kERROR_PAGE);
	}
	else if (this->current_context_.top() == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().error_page_list.push_back(tmp_err_pg);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kERROR_PAGE);
	}

	ti_ += 2;
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
	std::string address_part = ipv4.substr(0, mask_pos);
	std::string mask_part = (mask_pos != std::string::npos) ? ipv4.substr(mask_pos + 1) : "";

	// 3. 文字列がIPv4の基本的な構造に従っているかを確認
	std::istringstream iss(address_part);
	std::string field;
	std::vector<std::string> fields;

	while (std::getline(iss, field, '.'))
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
		std::istringstream(field) >> value;
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
	std::istringstream iss(address_part);
	std::string field;
	std::vector<std::string> fields;

	while (std::getline(iss, field, '.'))
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
	ti_++;
	std::string	address = this->tokens_[ti_].value_;

	if (address != "all" && !isIPv4(address) && !isIPv6(address))
	{
		std::cerr << "webserv: [emerg] invalid parameter \"" << address << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	config::Allow	tmp_allow;
	tmp_allow.setAddress(this->tokens_[ti_].value_);
	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.allow_list.push_back(tmp_allow);
		this->config_.http.directives_set.insert(kALLOW);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().allow_list.push_back(tmp_allow);
		this->config_.http.server_list.back().directives_set.insert(kALLOW);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().allow_list.push_back(tmp_allow);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kALLOW);
	}
	else if (context == config::CONF_HTTP_LIMIT_EXCEPT)
	{
		this->config_.http.server_list.back().location_list.back().limit_except.allow_list.push_back(tmp_allow);
		this->config_.http.server_list.back().location_list.back().limit_except.directives_set.insert(kALLOW);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseDeny()
{
	ti_++;
	std::string	address = this->tokens_[ti_].value_;

	if (address != "all" && !isIPv4(address) && !isIPv6(address))
	{
		std::cerr << "webserv: [emerg] invalid parameter \"" << address << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	config::Deny	tmp_deny;
	tmp_deny.setAddress(this->tokens_[ti_].value_);
	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.deny_list.push_back(tmp_deny);
		this->config_.http.directives_set.insert(kDENY);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().deny_list.push_back(tmp_deny);
		this->config_.http.server_list.back().directives_set.insert(kDENY);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().deny_list.push_back(tmp_deny);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kDENY);
	}
	else if (context == config::CONF_HTTP_LIMIT_EXCEPT)
	{
		this->config_.http.server_list.back().location_list.back().limit_except.deny_list.push_back(tmp_deny);
		this->config_.http.server_list.back().location_list.back().limit_except.directives_set.insert(kDENY);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseListen()
{
	ti_++;
	std::string					ori_val = this->tokens_[ti_].value_;
	config::Listen				tmp_listen;
	std::istringstream			iss;
	char						remaining_char;
	long						port;
	std::string					segment;
	std::vector<std::string>	segments;

	// 1. もし空文字列ならエラー
	if (ori_val.empty())
	{
			std::cerr << "webserv: [emerg] host not found in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	// 2. ip addressとportに分ける
	// ':'の領域が空なら空文字列を入れる。
	// ':'だけの場合はエラー
	if (ori_val == ":")
	{
			std::cerr << "webserv: [emerg] invalid port in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}
	if (ori_val[0] == ':')
	{
		segments.push_back("");
	}

	iss.str(ori_val.c_str());
	while (getline(iss, segment, ':'))
	{
		segments.push_back(segment);
	}  

	if (ori_val[ori_val.size() - 1] == ':')
	{
		segments.push_back("");
	}

	// 3. 2つ以上に分かれてしまっていたらエラー
	if (segments.size() > 2)
	{
		std::cerr << "webserv: [emerg] invalid parameter \"" << ori_val << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	// 4. ip address と portがある場合
	if (segments.size() == 2)
	{
		// ip addressがあれば値を確認する。
		// なければデフォルト値を入れる。
		if (segments[0].empty())
		{
			std::cerr << "webserv: [emerg] no host in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
		else if (!isIPv4(segments[0]) && !isIPv6(segments[0]))
		{
			std::cerr << "webserv: [emerg] host not found in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
		tmp_listen.setAddress(segments[0]);

		// port番号があれば値を確認しする。
		// なければデフォルト値を入れる。
		if (segments[1].empty())
		{
			std::cerr << "webserv: [emerg] invalid port in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
		else
		{
			iss.clear();
			iss.str(segments[1].c_str());
			if (iss >> port)
			{
				if (iss >> remaining_char)
				{
					std::cerr << "webserv: [emerg] host not found in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
					return false;
				}
				if (port < 0 || 65535 < port)
				{
					std::cerr << "webserv: [emerg] invalid port in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
					return false;
				}
			}
			else
			{
				std::cerr << "webserv: [emerg] host not found in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
		}
		tmp_listen.setPort(port);
	}
	else
	{
		// 5. ip addressかportのどちらかしかない場合
		iss.clear();
		iss.str(segments[0].c_str());

		// ip addressかportであればセット
		if (isIPv4(segments[0]) || isIPv6(segments[0]))
			tmp_listen.setAddress(segments[0]);
		else if (iss >> port)
		{
			if (iss >> remaining_char 
				|| port < 0
				|| 65535 < port)
			{
				std::cerr << "webserv: [emerg] invalid port in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
		}
		else
		{
			// addressでもportでもなければエラー
			std::cerr << "webserv: [emerg] host not found in \"" << ori_val << "\" of the \"listen\" directive in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
		tmp_listen.setPort(port);
	}

	// 6. defalt_serverがあるばあい
	if (this->tokens_[ti_ + 1].type_ != config::TK_SEMICOLON)
	{
		ti_++;

		if (this->tokens_[ti_].value_ == "default_server")
			tmp_listen.setIsDefaultServer(true);
		else
		{
			std::cerr << "webserv: [emerg] invalid parameter \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}
	}

	this->config_.http.server_list.back().listen_list.push_back(tmp_listen);
	this->config_.http.server_list.back().directives_set.insert(kLISTEN);

	ti_ += 2;
	return true;
}

bool	config::Parser::parseServerName()
{
	ti_++;

	// 最初のserver_nameディレクティブであれば、デフォルト値を削除する
	if (this->config_.http.server_list.back().directives_set.find(kSERVER_NAME) == this->config_.http.server_list.back().directives_set.end())
		this->config_.http.server_list.back().server_name.eraseDefaultName();

	while (this->tokens_[ti_].type_ != config::TK_SEMICOLON)
	{
		this->config_.http.server_list.back().server_name.addName(this->tokens_[ti_].value_);

		ti_++;
	}

	this->config_.http.server_list.back().directives_set.insert(kSERVER_NAME);
	ti_++;
	return true;
}

bool	config::Parser::parseTryFiles()
{
	ti_++;
	config::CONTEXT	context = this->current_context_.top();
	std::string		file;

	// 最後から2つ目までのトークンはfileとして追加
	while (this->tokens_[ti_ + 1].type_ != config::TK_SEMICOLON)
	{
		file = this->tokens_[ti_].value_;

		if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().try_files.addFile(file);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().try_files.addFile(file);

		ti_++;
	}

	// codeかuriか判定
	std::string			uri;
	long				code;
	std::istringstream	iss;
	char				remaining_char;

	if (this->tokens_[ti_].value_[0] == '=')
	{
		// code
		iss.str(this->tokens_[ti_].value_.substr(1));

		// code が正しいか判定
		if (iss >> code)
		{
			if (iss >> remaining_char
				|| code < 0
				|| 999 < code)
			{
				std::cerr << "webserv: [emerg] invalid code \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "webserv: [emerg] invalid code \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
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
		uri = this->tokens_[ti_].value_;

		// setUri
		if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().try_files.setUri(uri);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().try_files.setUri(uri);
	}

	if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().directives_set.insert(kTRY_FILES);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kTRY_FILES);

	ti_ += 2;
	return true;
}

bool	config::Parser::parseAlias()
{
	ti_++;
	std::string	path = this->tokens_[ti_].value_;

	this->config_.http.server_list.back().location_list.back().alias.setPath(path);

	ti_ += 2;
	return true;
}

bool	config::Parser::parseReturn()
{
	ti_++;
	long				code;
	std::string			url;
	std::istringstream	iss;
	char				remaining_char;
	config::Return		tmp_return;

	// もしトークンが2つあるなら一つ目はcodeなので処理する
	if (this->tokens_[ti_ + 2].type_ == config::TK_SEMICOLON)
	{
		iss.str(this->tokens_[ti_].value_.c_str());

		if (iss >> code)
		{
			if (iss >> remaining_char)
			{
				std::cerr << "webserv: [emerg] invalid return code \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}

			if (code < 0 || 999 < code)
			{
				std::cerr << "webserv: [emerg] invalid return code \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "webserv: [emerg] invalid return code \"" << this->tokens_[ti_].value_ << "\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}

		ti_++;
		tmp_return.setCode(code);
	}

	// url をセットする
	url = this->tokens_[ti_].value_;
	tmp_return.setUrl(url);

	this->config_.http.server_list.back().location_list.back().return_list.push_back(tmp_return);
	this->config_.http.server_list.back().location_list.back().directives_set.insert(kRETURN);

	ti_ += 2;
	return true;
}

bool	config::Parser::parseUserid()
{
	ti_++;
	std::string	tmp_switch = this->tokens_[ti_].value_;

	// もし、on/offではなかったらエラー
	if (tmp_switch != "on" && tmp_switch != "off")
	{
		std::cerr << "webserv: [emerg] invalid value " << tmp_switch << " in \"userid\" directive, it must be \"on\" or \"off\" in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	config::CONTEXT context = this->current_context_.top();

	// directives_setにセット
	if (context == config::CONF_HTTP)
		this->config_.http.directives_set.insert(kUSERID);
	else if (context == config::CONF_HTTP_SERVER)
		this->config_.http.server_list.back().directives_set.insert(kUSERID);
	else if (context == config::CONF_HTTP_LOCATION)
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kUSERID);

	// もし、onであれば、trueにする
	if (tmp_switch == "on")
	{
		if (context == config::CONF_HTTP)
			this->config_.http.userid.setIsUseridOn(true);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().userid.setIsUseridOn(true);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().userid.setIsUseridOn(true);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseUseridDomain()
{
	ti_++;
	std::string		name = this->tokens_[ti_].value_;
	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.userid_domain.setName(name);
		this->config_.http.directives_set.insert(kUSERID_DOMAIN);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().userid_domain.setName(name);
		this->config_.http.server_list.back().directives_set.insert(kUSERID_DOMAIN);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().userid_domain.setName(name);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kUSERID_DOMAIN);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseUseridExpires()
{
	ti_++;
	std::string	tmp_switch = this->tokens_[ti_].value_;
	config::CONTEXT	context = this->current_context_.top();

	// off であれば、なにもしない
	if (tmp_switch == "off")
	{
		if (context == config::CONF_HTTP)
			this->config_.http.directives_set.insert(kUSERID_EXPIRES);
		else if (context == config::CONF_HTTP_SERVER)
			this->config_.http.server_list.back().directives_set.insert(kUSERID_EXPIRES);
		else if (context == config::CONF_HTTP_LOCATION)
			this->config_.http.server_list.back().location_list.back().directives_set.insert(kUSERID_EXPIRES);

		ti_ += 2;
		return true;
	}

	long time = parseTime();
	if (time == -1)
	{
		std::cerr << "webserv: [emerg] \"client_max_body_size\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	if (context == config::CONF_HTTP)
	{
		this->config_.http.userid_expires.setTime(time);
		this->config_.http.userid_expires.setIsUseridExpiresOn(true);
		this->config_.http.directives_set.insert(kUSERID_EXPIRES);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().userid_expires.setTime(time);
		this->config_.http.server_list.back().userid_expires.setIsUseridExpiresOn(true);
		this->config_.http.server_list.back().directives_set.insert(kUSERID_EXPIRES);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().userid_expires.setTime(time);
		this->config_.http.server_list.back().location_list.back().userid_expires.setIsUseridExpiresOn(true);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kUSERID_EXPIRES);
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseUseridPath()
{
	ti_++;
	std::string	path = this->tokens_[ti_].value_;
	config::CONTEXT	context = this->current_context_.top();

	// もし値が空文字列でなければセットする
	if (!path.empty())
	{
		if (context == config::CONF_HTTP)
		{
			this->config_.http.userid_path.setPath(path);
			this->config_.http.directives_set.insert(kUSERID_PATH);
		}
		else if (context == config::CONF_HTTP_SERVER)
		{
			this->config_.http.server_list.back().userid_path.setPath(path);
			this->config_.http.server_list.back().directives_set.insert(kUSERID_PATH);
		}
		else if (context == config::CONF_HTTP_LOCATION)
		{
			this->config_.http.server_list.back().location_list.back().userid_path.setPath(path);
			this->config_.http.server_list.back().location_list.back().directives_set.insert(kUSERID_PATH);
		}
	}

	ti_ += 2;
	return true;
}

bool	config::Parser::parseUseridService()
{
	ti_++;
	long				user_id;
	std::istringstream	iss(this->tokens_[ti_].value_.c_str());
	char				remaining_char;

	if (iss >> user_id)
	{
		if (iss >> remaining_char)
		{
			std::cerr << "webserv: [emerg] \"userid_service\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
			return false;
		}

	}
	else
	{
		std::cerr << "webserv: [emerg] \"userid_service\" directive invalid value in " << this->filepath_ << ":" << this->tokens_[ti_].line_ << std::endl;
		return false;
	}

	config::CONTEXT	context = this->current_context_.top();

	if (context == config::CONF_HTTP)
	{
		this->config_.http.userid_service.setUseridService(user_id);
		this->config_.http.directives_set.insert(kUSERID_SERVICE);
	}
	else if (context == config::CONF_HTTP_SERVER)
	{
		this->config_.http.server_list.back().userid_service.setUseridService(user_id);
		this->config_.http.server_list.back().directives_set.insert(kUSERID_SERVICE);
	}
	else if (context == config::CONF_HTTP_LOCATION)
	{
		this->config_.http.server_list.back().location_list.back().userid_service.setUseridService(user_id);
		this->config_.http.server_list.back().location_list.back().directives_set.insert(kUSERID_SERVICE);
	}

	ti_ += 2;
	return true;
}

const config::Main	&config::Parser::getConfig() const
{
	return this->config_;
}

