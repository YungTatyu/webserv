#include "Parser.hpp"
#include <iostream>
#include <utility>
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
	return true;
}

const config::Main	&config::Parser::getConfig() const
{
	return this->config_;
}
