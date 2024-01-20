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

	this->all_directives_.insert("");
	(void) this->current_context_;

}

config::Parser::~Parser() {}

bool	config::Parser::expect(const config::TK_TYPE type)
{
	return this->tokens_[ti].type_ == type;
}