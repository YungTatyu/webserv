#include "Parser.hpp"
#include <iostream>
#include <utility>

std::map<std::string, unsigned int>	config::Parser::all_contexts_;
std::map<std::string, unsigned int>	config::Parser::all_directives_;

const unsigned int	config::Http::type;
const unsigned int	config::Server::type;
const unsigned int	config::Location::type;
const unsigned int	config::LimitExcept::type;
const unsigned int	config::AccessLog::kType_;

config::Parser::Parser(const std::vector<Token> &tokens, const std::string &filepath) :
	tokens_(tokens), filepath_(filepath), ti(0), current_context_(CONF_MAIN)
{
	this->all_directives_.insert(std::make_pair("main", CONF_MAIN));
	this->all_directives_.insert(std::make_pair("http", config::Http::type));
	this->all_directives_.insert(std::make_pair("server", config::Server::type));
	this->all_directives_.insert(std::make_pair("location", config::Location::type));
	this->all_directives_.insert(std::make_pair("limit_except", config::LimitExcept::type));
	this->all_directives_.insert(std::make_pair("access_log", config::AccessLog::kType_));

	// this->all_directives_.insert("access_log");
	// this->all_directives_.insert("alias");
	// this->all_directives_.insert("allow");
	// this->all_directives_.insert("autoindex");
	// this->all_directives_.insert("client_max_body_size");
	// this->all_directives_.insert("deny");
	// this->all_directives_.insert("error_log");
	// this->all_directives_.insert("error_page");
	// this->all_directives_.insert("index");
	// this->all_directives_.insert("keepalive_timeout");
	// this->all_directives_.insert("listen");
	// this->all_directives_.insert("return");
	// this->all_directives_.insert("root");
	// this->all_directives_.insert("send_timeout");
	// this->all_directives_.insert("server_name");
	// this->all_directives_.insert("try_files");
	// this->all_directives_.insert("use");
	// this->all_directives_.insert("userid");
	// this->all_directives_.insert("userid_domain");
	// this->all_directives_.insert("userid_expires");
	// this->all_directives_.insert("userid_path");
	// this->all_directives_.insert("userid_service");
	// this->all_directives_.insert("worker_connections");

	this->directives_parser_map_["access_log"] = &config::Parser::parseAccessLog;
}

config::Parser::~Parser() {}

/**
 * parseの流れ
 * 1. 存在するdirectiveか
 * 2. contextが正しいか
 * 3. argsの数が正しいか
 * 4. 重複を確認
 * 5. argsの値
*/
bool	config::Parser::parse()
{
	while (ti < this->tokens_.size())
	{
		const Token &current_token = this->tokens_[ti];
		// if (current_token.type_ == TK_END)
		// 	break;
		// TK_STRで絶対にはじまっている
		if (!expect(TK_STR))
			return false;
		// 存在するcontextまたはdirectiveか
		if (!isContext(current_token) && !isDirective(current_token))
		{
			printError(std::string("unknown directive ") + "\"" + current_token.value_ + "\"");
			return false;
		}
		if (!parseType(current_token.value_))
			return false;
	}
	if (this->set_contexts_.find("events") == this->set_contexts_.end())
	{
		std::cerr << "webserv: [emerg] no \"events\" section in configuration\n";
		return false;
	}
	return true;
}

bool	config::Parser::parseType(const std::string &directive)
{
	// contextが正しいか
	if (this->all_contexts_[directive] & this->current_context_)
		return false;

	// argsの数が正しいか

	return true;
}

bool	config::Parser::expect(const config::TK_TYPE type)
{
	if (this->tokens_[ti].type_ != type)
	{
		printError(std::string("unexpected ") + "\"" + this->tokens_[ti].value_ + "\"");
		return false;
	}
	return true;
}

bool	config::Parser::isContext(const config::Token &token)
{
	return token.type_ == config::TK_STR && this->all_contexts_.find(token.value_) != this->all_contexts_.end();
}

bool	config::Parser::isDirective(const config::Token &token)
{
	return token.type_ == config::TK_STR && this->all_directives_.find(token.value_) != this->all_directives_.end();
}

void	config::Parser::printError(const std::string &err_msg) const
{
	std::cerr << "webserv: [emerg] " << err_msg << " in " + this->filepath_ << ":" << this->tokens_[ti].line_  << '\n';
}

bool	config::Parser::parseAccessLog()
{
	return true;
}
