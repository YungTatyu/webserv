#include "Parser.hpp"

std::set<std::string>	config::Parser::all_contexts_;
std::set<std::string>	config::Parser::all_directives_;

// const std::set<std::string>	config::Parser::all_contexts_ = {
// 	"main",
// 	"http",
// 	"server",
// 	"location",
// 	"limit_except",
// };

// const std::set<std::string>	config::Parser::all_directives_ = {
// 	"",
// };


config::Parser::Parser(const std::vector<Token> &tokens) : tokens_(tokens), ti(0), current_context_(CONF_MAIN)
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
}

config::Parser::~Parser() {}

bool	config::Parser::expect(const config::TK_TYPE type)
{
	return this->tokens_[ti].type_ == type;
}

bool	config::Parser::is_context(const config::Token &token)
{
	return this->all_contexts_.find(token.value_) != this->all_contexts_.end();
}

bool	config::Parser::is_directive(const config::Token &token)
{
	return this->all_directives_.find(token.value_) != this->all_directives_.end();
}
