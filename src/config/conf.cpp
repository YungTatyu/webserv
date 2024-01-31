#include "conf.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include "Parser.hpp"
#include <limits>
#include <sys/param.h>
#include <stdlib.h>

const char	*config::Root::kDefaultPath_ = "html";
const char	*config::UseridPath::kDefaultPath_ = "/";
const unsigned long	config::SendTimeout::kDefaultTime_ = Time::seconds * 60; // 60s
const char	*config::Listen::kDefaultAddress_ = "127.0.0.1";
const char	*config::ServerName::kDefaultName_ = "";
const unsigned long	config::Size::kMaxSizeInBytes_ = std::numeric_limits<long>::max();
const char	*config::UseridDomain::kDefaultName_ = "none";
const char	*config::AccessLog::kDefaultFile_ = "logs/access.log";
const char	*config::ErrorLog::kDefaultFile_ = "logs/error.log";
const unsigned long	config::KeepaliveTimeout::kDefaultTime_ = 60 * Time::seconds; // 60s
const char	*config::Index::kDefaultFile_ = "index.html";

bool	config::init_config(const std::string& file_path)
{
	char	absolute_path[MAXPATHLEN];

	// 絶対pathを取得
	if (realpath(file_path.c_str(), absolute_path) == NULL)
	{
		std::cerr << "webserv: [emerg] realpath() \"" << file_path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return false;
	}

	// file_path が存在するかどうか
	if (access(absolute_path, F_OK))
	{
		std::cerr << "webserv: [emerg] access() \"" << absolute_path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return false;
	}

	// file_path の読み取り権限があるかどうか？ 
	if (access(absolute_path, R_OK))
	{
		std::cerr << "webserv: [emerg] access() \"" << absolute_path << "\" failed (" << errno << ": " << strerror(errno) << ")"<< std::endl;
		return false;
	}

	// file_path がファイルかどうか確認する。
	struct stat fileInfo;

	if (stat(absolute_path, &fileInfo) == 0 && !S_ISREG(fileInfo.st_mode))
	{
		std::cerr << "webserv: [crit] \"" << absolute_path << "\" is a directory" << std::endl;
		return false;
	}

	config::Lexer lexer(absolute_path);
	lexer.tokenize();
	// std::cout << "websev: [debug] tokenize() succeeded" << std::endl;
	const std::vector<Token>	&tokens = lexer.getTokens();
	config::Parser	parser(tokens, absolute_path);
	if (!parser.parse())
		return false;
	//std::cout << "websev: [debug] parse() succeeded" << std::endl;

	return true;
}

