#include "conf.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

bool	config::init_config(const std::string& file_path)
{
	// file_path が存在するかどうか
	if (access(file_path.c_str(), F_OK))
	{
		std::cerr << "webserv: [emerg] access() \"" << file_path << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return false;
	}

	// file_path の読み取り権限があるかどうか？ 
	if (access(file_path.c_str(), R_OK))
	{
		std::cerr << "webserv: [emerg] access() \"" << file_path << "\" failed (" << errno << ": " << strerror(errno) << ")"<< std::endl;
		return false;
	}

	// file_path がファイルかどうか確認する。
	struct stat fileInfo;

	if (stat(file_path.c_str(), &fileInfo) == 0 && !S_ISREG(fileInfo.st_mode))
	{
		std::cerr << "webserv: [crit] \"" << file_path << "\" is a directory" << std::endl;
		return false;
	}

	config::Lexer lexer(file_path);
	lexer.tokenize();
	std::cout << "websev: [debug] tokenize() succeeded" << std::endl;

	return true;
}

