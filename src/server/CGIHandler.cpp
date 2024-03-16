#include "CGIHandler.hpp"
#include "FileUtils.hpp"
#include "SysCallWrapper.hpp"

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>

cgi::CGIHandler::CGIHandler() : cgi_process_id_(-1) {}

cgi::CGIHandler::~CGIHandler() {}

/**
 * @brief cgi実行fileか
 * cgi実行ファイルの拡張子
 * 1. *.php
 * 2. *.cig
 * 3. *.py
 * 
 * @param cgi_path 
 * @return true 
 * @return false 
 */
bool cgi::CGIHandler::isCgi(const std::string& cgi_path)
{
	return (
		FileUtils::isExtensionFile(cgi_path, ".php")
		|| FileUtils::isExtensionFile(cgi_path, ".cgi")
		|| FileUtils::isExtensionFile(cgi_path, ".py")
	) && FileUtils::isExecutable(cgi_path.c_str());
}

/**
 * @brief cgiプロセスを複製し、cgiを実行する
 * 
 * @return true 
 * @return false 
 */
bool	cgi::CGIHandler::forkCgiProcess(
	const HttpRequest& http_request,
	const std::string& cgi_path,
	const int cli_socket,
	ConnectionManager& conn_manager
)
{
	pid_t	pid = fork();
	if (pid == -1)
	{
		std::cerr << "webserv: [emerg] fork() failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return false;
	}
	if (pid == 0)
	{
		close(this->sockets_[SOCKET_PARENT]);
		this->cgi_executor_.executeCgi(http_request, cgi_path, this->sockets_[SOCKET_CHILD]);
	}
	this->cgi_process_id_ = pid;
	close(this->sockets_[SOCKET_CHILD]);
	// すでに登録されているクライアントソケットのREAD EVENTは削除する必要がある（特にkqueue server）
	conn_manager.setEvent(cli_socket, ConnectionData::EV_CGI_READ);
	// timertreeにtimeoutを追加
	return true;
}

void	cgi::CGIHandler::callCgiExecutor(
	const std::string& cgi_path,
	const HttpRequest& http_request,
	const int cli_socket,
	ConnectionManager& conn_manager
)
{
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockets_) == -1)
	{
		std::cerr << "webserv: [emerg] socketpair() failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
		return;
	}
	forkCgiProcess(http_request, cgi_path, cli_socket, conn_manager);
}

const cgi::CGIParser&	cgi::CGIHandler::getCgiParser() const
{
	return this->cgi_parser_;
}

const cgi::CGIExecutor&	cgi::CGIHandler::getCgiExecutor() const
{
	return this->cgi_executor_;
}

/**
 * @brief timeout eventが発生した場合、cgi processをkillする
 * 
 */
void	cgi::CGIHandler::killCgiProcess() const
{
	if (kill(this->cgi_process_id_, SIGINT) == -1)
		std::cerr << "webserv: [emerg] kill() failed (" << errno << ": " << strerror(errno) << ")" << std::endl;
}
