#include "Parser.hpp"
#include <iostream>
#include <utility>

std::map<std::string, unsigned int>	config::Parser::all_contexts_;
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

config::Parser::Parser(const std::vector<Token> &tokens, const std::string &filepath) :
	tokens_(tokens), filepath_(filepath), ti(0), current_context_(CONF_MAIN)
{
	// context
	this->all_directives_.insert(std::make_pair("main", CONF_MAIN));
	this->all_directives_.insert(std::make_pair("events", config::Events::type));
	this->all_directives_.insert(std::make_pair("http", config::Http::type));
	this->all_directives_.insert(std::make_pair("server", config::Server::type));
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
	this->parser_map_["access_log"] = &config::Parser::parseAccessLog;
}

config::Parser::~Parser() {}

/**
 * parseの流れ
 * 1. 存在するdirectiveか
 * 2. argsが想定されていないtokenではないか："{", "}"
 * 3. contextが正しいか
 * 4. argsの数が正しいか
 * 5. 重複を確認
 * 6. argsの値
*/
bool	config::Parser::parse()
{
	while (ti < this->tokens_.size())
	{
		const Token &current_token = this->tokens_[ti];
		// if (current_token.type_ == TK_END)
		// 	break;
		// TK_STRで絶対にはじまっている
		if (!expectTokenType(TK_STR, current_token))
			return false;
		// 存在するcontextまたはdirectiveか
		if (!isDirective(current_token) && !isContext(current_token))
		{
			printError(std::string("unknown directive ") + "\"" + current_token.value_ + "\"", current_token);
			return false;
		}
		// contextとargsの数が正しいか
		if (!parseType(current_token))
			return false;
		// 重複の確認
		
		// directiveのargsの値を確認
		if (!this->parser_map_[current_token.value_])
			return false;
	}
	// events contextが設定されていないとparse error
	if (this->set_contexts_.find("events") == this->set_contexts_.end())
	{
		std::cerr << "webserv: [emerg] no \"events\" section in configuration\n";
		return false;
	}
	return true;
}

bool	config::Parser::parseType(const Token &token)
{
	const std::string directive_name = token.value_;
	// contextが正しいか
	if (!(this->all_directives_[directive_name] & this->current_context_))
	{
		printError(std::string("\"") + directive_name + "\" directive is not allowed here", token);
		return false;
	}
	// argsの数が正しいか
	const TK_TYPE terminating_token = isContext(token) ? TK_OPEN_CURLY_BRACE : TK_SEMICOLON;
	const unsigned int	args_num = static_cast<unsigned int>(countArgs(terminating_token));
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
		ret = expectArgsNum(CONF_TAKE2|CONF_2MORE, this->all_directives_[directive_name]);
		break;
	case 3:
		ret = expectArgsNum(CONF_TAKE3, this->all_directives_[directive_name]);
		break;
	case 4:
		ret = expectArgsNum(CONF_TAKE4, this->all_directives_[directive_name]);
		break;
	case 5:
		ret = expectArgsNum(CONF_TAKE5, this->all_directives_[directive_name]);
		break;
	case 6:
		ret = expectArgsNum(CONF_TAKE6, this->all_directives_[directive_name]);
		break;
	case 7:
		ret = expectArgsNum(CONF_TAKE7, this->all_directives_[directive_name]);
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
	return true;
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

bool	config::Parser::isContext(const config::Token &token) const
{
	return token.type_ == config::TK_STR &&
	(
		token.value_ == "main"
		|| token.value_ == "events"
		|| token.value_ == "http"
		|| token.value_ == "server"
		|| token.value_ == "location"
		|| token.value_ == "limit_except"
	);
}

bool	config::Parser::isDirective(const config::Token &token) const
{
	return token.type_ == config::TK_STR
		&& !isContext(token)
		&& this->all_directives_.find(token.value_) != this->all_directives_.end();
}

/**
 * tokenを一時的に進める必要があるため、引数でtokenを渡さない
 * @date terminating_token: directive、contextの終了条件
 * directive ;
 * context   {
 * 
*/
ssize_t	config::Parser::countArgs(const TK_TYPE terminating_token) const
{
	ssize_t	i = this->ti;

	while (this->tokens_[i].type_ != terminating_token)
	{
		// if (this->tokens_[i].type_ == TK_END)
		// {
		// 	printError("unexpected end of file, expecting \";\" or \"}\"");
		// 	return -1;
		// }
		if (expectTokenType(TK_STR, this->tokens_[i]))
		{
			printError(std::string("unexpected \"") + this->tokens_[i].value_ + "\"", this->tokens_[ti]);
			return -1;
		}
		++i;
	}
	return i;
}

void	config::Parser::printError(const std::string &err_msg, const Token &token) const
{
	std::cerr << "webserv: [emerg] " << err_msg << " in " + this->filepath_ << ":" << token.line_  << '\n';
}

bool	config::Parser::parseAccessLog()
{
	return true;
}
