#include "LogFd.hpp"
#include "FileUtils.hpp"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

const static	std::string kACCESS_LOG = "access_log";
const static	std::string kACCESS_FD = "access_fd";
const static	std::string kERROR_LOG = "error_log";
const static	std::string kERROR_FD = "error_fd";

/*
 * AddAcsFdList/AddErrFdListの返り値
 *
 * 1:  fdを追加する
 * 0:  log directiveがないので、fdを追加しない
 * -1: error
 */
int	config::addAcsFdList ( std::set<std::string>& directives_set, const std::vector<config::AccessLog>& access_log_list, std::vector<int>& fd_list )
{
	// access_log directiveがなければ飛ばす。
	if (directives_set.find(kACCESS_LOG) == directives_set.end())
		return 0;

	std::string	tmp_path;
	int	tmp_fd;

	for (size_t i = 0; i < access_log_list.size(); i++)
	{
		// access_logはそのコンテキスト中にoffがあればすべて無効
		if (!access_log_list[i].getIsAccesslogOn())
		{
			fd_list.clear();
			return 1;
		}
		tmp_path = access_log_list[i].getFile();
		// ファイルはあるが、write権限がない時ときは飛ばす
		// ここのエラー出力任意にできるようにする。でないと、ファイルがない時は毎回accessエラーでる
		if (FileUtils::wrapperAccess(tmp_path, F_OK, false) == 0 && FileUtils::wrapperAccess(tmp_path, W_OK, false) == -1)
			continue;
		// openするのはそのディレクティブにエラーやoffがないことがわかってからの方が無駄なファイルつくらなくて済む
		tmp_fd = FileUtils::wrapperOpen(tmp_path, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
		if (tmp_fd == -1)
		{
			return -1;
		}
		fd_list.push_back(tmp_fd);
	}
	directives_set.insert(kACCESS_FD);
	return 1;
}

int	config::addErrFdList ( std::set<std::string>& directives_set, const std::vector<config::ErrorLog>& error_log_list, std::vector<int>& fd_list )
{
	// error_log directiveがなければ飛ばす。
	if (directives_set.find(kERROR_LOG) == directives_set.end())
		return 0;

	std::string	tmp_path;
	int	tmp_fd;

	for (size_t i = 0; i < error_log_list.size(); i++)
	{
		tmp_path = error_log_list[i].getFile();
		// ファイルはあるが、write権限がない時ときは飛ばす
		// ここのエラー出力任意にできるようにする。でないと、ファイルがない時は毎回accessエラーでる
		if (FileUtils::wrapperAccess(tmp_path, F_OK, false) == 0 && FileUtils::wrapperAccess(tmp_path, W_OK, false) == -1)
			continue;
		tmp_fd = FileUtils::wrapperOpen(tmp_path, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
		if (tmp_fd == -1)
		{
			return -1;
		}
		fd_list.push_back(tmp_fd);
	}

	directives_set.insert(kERROR_FD);
	return 1;
}

/* 各コンテキストのaccess_logディレクティブで指定されたファイルのfdを取得する
 */
bool	config::initAcsLogFds( config::Main& config )
{
	// http context
	int ret = addAcsFdList(config.http.directives_set, config.http.access_log_list, config.http.access_fd_list);
	if (ret == -1)
		return false;
	else if (ret == 0)
	{
		std::string	absolute_path;
		if (!FileUtils::wrapperRealpath(".", absolute_path))
		{
			std::cerr << "webserv: [emerg] realpath() \"" << "." <<"\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
			return false;
		}
		int tmp_fd = FileUtils::wrapperOpen(absolute_path + "/" + static_cast<std::string>(config::AccessLog::kDefaultFile_), O_WRONLY | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
		if (tmp_fd == -1)
		{
			return false;
		}
		config.http.access_fd_list.push_back(tmp_fd);
		config.http.directives_set.insert(kACCESS_FD);
	}

	// server context
	for (size_t si = 0; si < config.http.server_list.size(); si++)
	{
		if (addAcsFdList(config.http.server_list[si].directives_set, config.http.server_list[si].access_log_list, config.http.server_list[si].access_fd_list) == -1)
			return false;

		// location context
		for (size_t li = 0; li < config.http.server_list[si].location_list.size(); li++)
		{
			if (addAcsFdList(config.http.server_list[si].location_list[li].directives_set, config.http.server_list[si].location_list[li].access_log_list, config.http.server_list[si].location_list[li].access_fd_list) == -1)
				return false;
		}
	}

	return true;
}

/* 各コンテキストのerror_logディレクティブで指定されたファイルのfdを取得する
 */
bool	config::initErrLogFds( config::Main& config )
{
	// main context
	int ret = addErrFdList(config.directives_set, config.error_log_list, config.error_fd_list);
	if (ret == -1)
		return false;
	else if (ret == 0)
	{
		std::string	absolute_path;
		if (!FileUtils::wrapperRealpath(".", absolute_path))
		{
			std::cerr << "webserv: [emerg] realpath() \"" << "." << "\" failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
			return false;
		}

		int tmp_fd = FileUtils::wrapperOpen(absolute_path + "/" + static_cast<std::string>(config::ErrorLog::kDefaultFile_), O_WRONLY | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
		if (tmp_fd == -1)
		{
			return false;
		}
		config.error_fd_list.push_back(tmp_fd);
		config.directives_set.insert(kERROR_FD);
	}

	// http context
	if (addErrFdList(config.http.directives_set, config.http.error_log_list, config.http.error_fd_list) == -1)
		return false;

	// server context
	for (size_t si = 0; si < config.http.server_list.size(); si++)
	{
		if (addErrFdList(config.http.server_list[si].directives_set, config.http.server_list[si].error_log_list, config.http.server_list[si].error_fd_list) == -1)
			return false;

		// location context
		for (size_t li = 0; li < config.http.server_list[si].location_list.size(); li++)
		{
			if (addErrFdList(config.http.server_list[si].location_list[li].directives_set, config.http.server_list[si].location_list[li].error_log_list, config.http.server_list[si].location_list[li].error_fd_list) == -1)
				return false;
		}
	}

	return true;
}

bool	config::initLogFds( config::Main& config )
{
	if (!initAcsLogFds(config) || !initErrLogFds(config))
		return false;
	return true;
}

void	config::closeLogFds( const std::vector<int> log_list )
{
	for (size_t i = 0; i < log_list.size(); i++)
		close(log_list[i]);
}

void config::terminateLogFds( const config::Main* config )
{
	// main context
	closeLogFds(config->error_fd_list);

	// http context
	closeLogFds(config->http.access_fd_list);
	closeLogFds(config->http.error_fd_list);

	// server context
	for (size_t i = 0; i < config->http.server_list.size(); i++)
	{
		closeLogFds(config->http.server_list[i].access_fd_list);
		closeLogFds(config->http.server_list[i].error_fd_list);

		for (size_t j = 0; j < config->http.server_list[i].location_list.size(); j++)
		{
			closeLogFds(config->http.server_list[i].location_list[j].access_fd_list);
			closeLogFds(config->http.server_list[i].location_list[j].error_fd_list);
		}
	}
}

