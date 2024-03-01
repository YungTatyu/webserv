#include "conf.hpp"
#include "InitLogFd.hpp"
#include "Lexer.hpp"
#include "Main.hpp"
#include "Parser.hpp"
#include "FileUtils.hpp"
#include <fstream>
#include <iostream>
#include <limits>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

const char	*config::Root::kDefaultPath_ = "html";
const char	*config::UseridPath::kDefaultPath_ = "/";
const unsigned long	config::SendTimeout::kDefaultTime_ = 60 * Time::seconds; // 60s
const int	config::Return::kRedirectCodes_[] = {301, 302, 302, 307, 308};
const char	*config::Listen::kDefaultAddress_ = "127.0.0.1";
const char	*config::ServerName::kDefaultName_ = "";
const unsigned long	config::Time::kMaxTimeInMilliseconds_ = std::numeric_limits<long>::max();
const unsigned long	config::Size::kMaxSizeInBytes_ = std::numeric_limits<long>::max();
const char	*config::UseridDomain::kDefaultName_ = "none";
const char	*config::AccessLog::kDefaultFile_ = "logs/access.log";
const char	*config::ErrorLog::kDefaultFile_ = "logs/error.log";
const unsigned long	config::KeepaliveTimeout::kDefaultTime_ = 60 * Time::seconds; // 60s
const char	*config::Index::kDefaultFile_ = "index.html";

config::Main	*config::initConfig( const std::string& file_path )
{
	std::string	absolute_path;

	// 絶対pathを取得
	absolute_path = FileUtils::deriveAbsolutePath(file_path);
	if (absolute_path == "")
		return NULL;

	// file_path が存在するかどうか
	if (access(absolute_path.c_str(), F_OK))
	{
		std::cerr << "webserv: [emerg] access() \"" << absolute_path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return NULL;
	}

	// file_path の読み取り権限があるかどうか？ 
	if (access(absolute_path.c_str(), R_OK))
	{
		std::cerr << "webserv: [emerg] access() \"" << absolute_path << "\" failed (" << errno << ": " << strerror(errno) << ")"<< std::endl;
		return NULL;
	}

	// file_path がファイルかどうか確認する。
	struct stat fileInfo;

	if (stat(absolute_path.c_str(), &fileInfo) == 0 && !S_ISREG(fileInfo.st_mode))
	{
		std::cerr << "webserv: [crit] \"" << absolute_path << "\" is a directory" << std::endl;
		return NULL;
	}

	config::Lexer lexer(absolute_path);
	lexer.tokenize();
	const std::vector<Token>	&tokens = lexer.getTokens();

	Main	*config = new Main();
	config::Parser	parser(*config, tokens, absolute_path);
	if (!parser.parse())
	{
		delete config;
		return NULL;
	}

	if (!initLogFds(*config))
	{
		delete config;
		return NULL;
	}

	return config;
}
