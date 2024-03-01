#include "InitLogFd.hpp"
#include "FileUtils.hpp"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

const static	std::string kACCESS_LOG = "access_log";
const static	std::string kACCESS_FD = "access_fd";
const static	std::string kERROR_LOG = "error_log";
const static	std::string kERROR_FD = "error_fd";

bool	config::addAcsFdList( std::vector<int>& fd_list, const std::vector<config::AccessLog>& access_log_list )
{
	std::string	tmp_path;
	int	tmp_fd;

	for (size_t i = 0; i < access_log_list.size(); i++)
	{
		tmp_path = access_log_list[i].getFile();
		// access_logはそのコンテキスト中にoffがあればすべて無効
		if (tmp_path == "off")
		{
			fd_list.clear();
			return true;
		}
		// access権限がない時ときは落ちないようにする
		tmp_fd = FileUtils::wrapperOpen(tmp_path, O_WRONLY | O_CREAT, S_IWUSR);
		if (tmp_fd == -1)
			return false;
		fd_list.push_back(tmp_fd);
	}

	return true;
}

bool	config::addErrFdList( std::vector<int>& fd_list, const std::vector<config::ErrorLog>& error_log_list )
{
	std::string	tmp_path;
	int	tmp_fd;

	for (size_t i = 0; i < error_log_list.size(); i++)
	{
		tmp_path = error_log_list[i].getFile();
		// access権限がない時ときは落ちないようにする
		tmp_fd = FileUtils::wrapperOpen(tmp_path, O_WRONLY | O_CREAT, S_IWUSR);
		if (tmp_fd == -1)
			return false;
		fd_list.push_back(tmp_fd);
	}

	return true;
}

/* 各コンテキストのaccess_logディレクティブで指定されたファイルのfdを取得する
 */
bool	config::initAcsLogFds( config::Main& config )
{
	// http context
	if (config.http.directives_set.find(kACCESS_LOG) != config.http.directives_set.end())
	{
		if (!addAcsFdList(config.http.access_fd_list, config.http.access_log_list))
			return false;
		config.http.directives_set.insert(kACCESS_FD);
	}
	else
	{
		int tmp_fd = FileUtils::wrapperOpen(FileUtils::deriveAbsolutePath("../../") + "logs/access_log", O_CREAT | O_WRONLY, S_IWRITE);
		if (tmp_fd == -1)
			return false;
		config.http.access_fd_list.push_back(tmp_fd);
		config.http.directives_set.insert(kACCESS_FD);
	}

	// server context
	for (size_t i = 0; i < config.http.server_list.size(); i++)
	{
		if (config.http.server_list[i].directives_set.find(kACCESS_LOG) != config.http.server_list[i].directives_set.end())
		{
			if (!addAcsFdList(config.http.server_list[i].access_fd_list, config.http.server_list[i].access_log_list))
				return false;
			config.http.server_list[i].directives_set.insert(kACCESS_FD);
		}
	}

	// location context
	for (size_t i = 0; i < config.http.server_list.size(); i++)
	{
		for (size_t j = 0; j < config.http.server_list[i].location_list.size(); j++)
		{
			if (config.http.server_list[i].location_list[j].directives_set.find(kACCESS_LOG) != config.http.server_list[i].location_list[j].directives_set.end())
			{
				if (!addAcsFdList(config.http.server_list[i].location_list[j].access_fd_list, config.http.server_list[i].location_list[j].access_log_list))
					return false;
				config.http.server_list[i].location_list[j].directives_set.insert(kACCESS_FD);
			}
		}
	}

	return true;
}

/* 各コンテキストのerror_logディレクティブで指定されたファイルのfdを取得する
 */
bool	config::initErrLogFds( config::Main& config )
{
	// main context
	if (config.directives_set.find(kERROR_LOG) != config.directives_set.end())
	{
		if (!addErrFdList(config.error_fd_list, config.http.error_log_list))
			return false;
		config.directives_set.insert(kERROR_FD);
	}
	else
	{
		int tmp_fd = FileUtils::wrapperOpen(FileUtils::deriveAbsolutePath("../../") + "logs/error_log", O_CREAT | O_WRONLY, S_IWRITE);
		if (tmp_fd == -1)
			return false;
		config.http.error_fd_list.push_back(tmp_fd);
		config.directives_set.insert(kERROR_FD);
	}

	// http context
	if (config.http.directives_set.find(kERROR_LOG) != config.http.directives_set.end())
	{
		if (!addErrFdList(config.http.error_fd_list, config.http.error_log_list))
			return false;
		config.http.directives_set.insert(kERROR_FD);
	}

	// server context
	for (size_t i = 0; i < config.http.server_list.size(); i++)
	{
		if (config.http.server_list[i].directives_set.find(kERROR_LOG) != config.http.server_list[i].directives_set.end())
		{
			if (!addErrFdList(config.http.server_list[i].error_fd_list, config.http.server_list[i].error_log_list))
				return false;
			config.http.server_list[i].directives_set.insert(kERROR_FD);
		}
	}

	// location context
	for (size_t i = 0; i < config.http.server_list.size(); i++)
	{
		for (size_t j = 0; j < config.http.server_list[i].location_list.size(); j++)
		{
			if (config.http.server_list[i].location_list[j].directives_set.find(kERROR_LOG) != config.http.server_list[i].location_list[j].directives_set.end())
			{
				if (!addErrFdList(config.http.server_list[i].location_list[j].error_fd_list, config.http.server_list[i].location_list[j].error_log_list))
					return false;
				config.http.server_list[i].location_list[j].directives_set.insert(kERROR_FD);
			}
		}
	}

	return true;
}

bool	config::initLogFds( config::Main& config )
{
	if (!initAcsLogFds(config) | !initErrLogFds(config))
		return false;
	return true;
}


