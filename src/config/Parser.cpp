#include "Parser.hpp"
#include <iostream>

std::set<std::string>	config::Parser::all_contexts_;
std::set<std::string>	config::Parser::all_directives_;

config::Parser::Parser(const std::vector<Token> &tokens, const std::string &filepath) :
	tokens_(tokens), filepath_(filepath), ti(0), current_context_(CONF_MAIN)
{
	this->all_contexts_.insert("main");
    this->all_contexts_.insert("http");
    this->all_contexts_.insert("server");
    this->all_contexts_.insert("location");
    this->all_contexts_.insert("limit_except");

	this->all_directives_.insert("access_log");
	this->all_directives_.insert("alias");
	this->all_directives_.insert("allow");
	this->all_directives_.insert("autoindex");
	this->all_directives_.insert("client_max_body_size");
	this->all_directives_.insert("deny");
	this->all_directives_.insert("error_log");
	this->all_directives_.insert("error_page");
	this->all_directives_.insert("index");
	this->all_directives_.insert("keepalive_timeout");
	this->all_directives_.insert("listen");
	this->all_directives_.insert("return");
	this->all_directives_.insert("root");
	this->all_directives_.insert("send_timeout");
	this->all_directives_.insert("server_name");
	this->all_directives_.insert("try_files");
	this->all_directives_.insert("use");
	this->all_directives_.insert("userid");
	this->all_directives_.insert("userid_domain");
	this->all_directives_.insert("userid_expires");
	this->all_directives_.insert("userid_path");
	this->all_directives_.insert("userid_service");
	this->all_directives_.insert("worker_connections");

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
		// TK_STRで絶対にはじまっている
		if (!expect(TK_STR))
			return false;
		const Token &current_token = this->tokens_[ti];
		// 存在するcontextまたはdirectiveか
		if (!isContext(current_token) && !isDirective(current_token))
		{
			printError(std::string("unknown directive " + '\"' + current_token.value_ + '\"'));
			return false;
		}
		if (!parseType(current_token.value_))
			return false;
	}
	return true;
}

bool	config::Parser::parseType(const std::string &directive)
{
	// contextが正しいか

	// argsの数が正しいか

	return true;
}

bool	config::Parser::expect(const config::TK_TYPE type)
{
	if (this->tokens_[ti].type_ != type)
	{
		printError(std::string("unexpected ") + '\"' + this->tokens_[ti].value_ + '\"');
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
	std::cerr << "webserv: [emerg] " << err_msg << " in " + this->filepath_ << ':' << this->tokens_[ti].line_  << '\n';
}
