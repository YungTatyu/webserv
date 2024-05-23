#include "CGIHandler.hpp"

#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "SysCallWrapper.hpp"
#include "Utils.hpp"

cgi::CGIHandler::CGIHandler() : cgi_process_id_(-1), cli_socket_(-1) { resetSockets(); }

cgi::CGIHandler::~CGIHandler() {}

/**
 * @brief cgi実行fileか
 * cgi実行ファイルの拡張子
 * 1. *.php
 * 2. *.cgi
 * 3. *.py
 *
 * @param script_path
 * @return true
 * @return false
 */
bool cgi::CGIHandler::isCgi(const std::string& script_path) {
  return (Utils::isExtensionFile(script_path, ".php") || Utils::isExtensionFile(script_path, ".cgi") ||
          Utils::isExtensionFile(script_path, ".py"));
}

/**
 * @brief cgiプロセスを複製し、cgiを実行する
 *
 * @return true
 * @return false
 */
bool cgi::CGIHandler::forkCgiProcess(const HttpRequest& request, const std::string& script_path, const std::string& path_info) {
  pid_t pid = SysCallWrapper::Fork();
  if (pid == -1) return false;
  if (pid == 0) {
    close(this->sockets_[SOCKET_PARENT]);
    this->cgi_executor_.executeCgiScript(request, script_path, path_info, this->sockets_[SOCKET_CHILD],
                                         this->cli_socket_);
  }
  this->cgi_process_id_ = pid;
  close(this->sockets_[SOCKET_CHILD]);
  return true;
}

bool cgi::CGIHandler::callCgiExecutor(const std::string& script_path, const std::string& path_info, const HttpRequest& request,
                                      const int cli_sock) {
  this->cli_socket_ = cli_sock;
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockets_) == -1) {
    std::cerr << "webserv: [emerg] socketpair() failed (" << errno << ": " << std::strerror(errno) << ")"
              << std::endl;
    return false;
  }
#if defined(SO_NOSIGPIPE)
  int opt = 1;
  SysCallWrapper::Setsockopt(this->sockets_[SOCKET_PARENT], SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

  Utils::setNonBlockingCloExec(this->sockets_[SOCKET_PARENT]);
  return forkCgiProcess(request, script_path, path_info);
}

bool cgi::CGIHandler::callCgiParser(HttpResponse& response, const std::string& cgi_response) {
  return this->cgi_parser_.parse(response, cgi_response, cgi::PARSE_BEFORE);
}

const cgi::CGIParser& cgi::CGIHandler::getCgiParser() const { return this->cgi_parser_; }

const cgi::CGIExecutor& cgi::CGIHandler::getCgiExecutor() const { return this->cgi_executor_; }

pid_t cgi::CGIHandler::getCgiProcessId() const { return this->cgi_process_id_; }

int cgi::CGIHandler::getCliSocket() const { return this->cli_socket_; }

void cgi::CGIHandler::setCliSocket(const int socket) { this->cli_socket_ = socket; }

int cgi::CGIHandler::getCgiSocket() const { return this->sockets_[SOCKET_PARENT]; }

/**
 * @brief timeout eventが発生した場合、cgi processをkillする
 *
 */
void cgi::CGIHandler::killCgiProcess() const {
  if (kill(this->cgi_process_id_, SIGINT) == -1)
    std::cerr << "webserv: [emerg] kill() failed (" << errno << ": " << std::strerror(errno) << ")"
              << std::endl;
}

/**
 * @brief socketの値と被らないように初期化する
 *
 */
void cgi::CGIHandler::resetSockets() {
  this->sockets_[0] = -2;
  this->sockets_[1] = -2;
}
