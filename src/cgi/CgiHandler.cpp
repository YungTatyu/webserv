#include "CgiHandler.hpp"

#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "WebServer.hpp"
#include "error.hpp"
#include "syscall_wrapper.hpp"
#include "utils.hpp"

cgi::CgiHandler::CgiHandler() : cgi_process_id_(-1), cli_socket_(-1) { resetSockets(); }

cgi::CgiHandler::~CgiHandler() {}

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
bool cgi::CgiHandler::isCgi(const std::string& script_path) {
  return (utils::isExtensionFile(script_path, ".php") || utils::isExtensionFile(script_path, ".cgi") ||
          utils::isExtensionFile(script_path, ".py"));
}

/**
 * @brief cgiプロセスを複製し、cgiを実行する
 *
 * @return true
 * @return false
 */
bool cgi::CgiHandler::forkCgiProcess(const HttpRequest& request, const HttpResponse& response) {
  pid_t pid = syscall_wrapper::Fork();
  if (pid == -1) {
    close(this->sockets_[SOCKET_PARENT]);
    close(this->sockets_[SOCKET_CHILD]);
    resetSockets();
    return false;
  }
  if (pid == 0) {
    close(this->sockets_[SOCKET_PARENT]);
    this->cgi_executor_.executeCgiScript(request, response, this->sockets_[SOCKET_CHILD], this->cli_socket_);
  }
  this->cgi_process_id_ = pid;
  close(this->sockets_[SOCKET_CHILD]);
  return true;
}

bool cgi::CgiHandler::callCgiExecutor(const HttpResponse& response, const HttpRequest& request,
                                      int cli_sock) {
  this->cli_socket_ = cli_sock;
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->sockets_) == -1) {
    WebServer::writeErrorlog(error::strSysCallError("socketpair"), config::EMERG);
    return false;
  }
#if defined(SO_NOSIGPIPE)
  int opt = 1;
  syscall_wrapper::Setsockopt(this->sockets_[SOCKET_PARENT], SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

  utils::setNonBlockCloExec(this->sockets_[SOCKET_PARENT]);
  return forkCgiProcess(request, response);
}

bool cgi::CgiHandler::callCgiParser(HttpResponse& response, const std::string& cgi_response) {
  return this->cgi_parser_.parse(response, cgi_response, cgi::PARSE_BEFORE);
}

const cgi::CgiParser& cgi::CgiHandler::getCgiParser() const { return this->cgi_parser_; }

const cgi::CgiExecutor& cgi::CgiHandler::getCgiExecutor() const { return this->cgi_executor_; }

pid_t cgi::CgiHandler::getCgiProcessId() const { return this->cgi_process_id_; }

int cgi::CgiHandler::getCliSocket() const { return this->cli_socket_; }

void cgi::CgiHandler::setCliSocket(int socket) { this->cli_socket_ = socket; }

int cgi::CgiHandler::getCgiSocket() const { return this->sockets_[SOCKET_PARENT]; }

/**
 * @brief timeout eventが発生した場合、cgi processをkillする
 *
 */
void cgi::CgiHandler::killCgiProcess() const {
  if (kill(this->cgi_process_id_, SIGKILL) == -1)
    WebServer::writeErrorlog(error::strSysCallError("kill"), config::EMERG);
}

/**
 * @brief socketの値と被らないように初期化する
 *
 */
void cgi::CgiHandler::resetSockets() {
  this->sockets_[0] = -1;
  this->sockets_[1] = -1;
}

/**
 * @brief cgi processが生きているか確認。死んでいたらstatusでexit status確認。
 *
 * @param process_id, status
 * @return true cgi processが死んでいる
 * @return false cgi processがまだ生きている
 */
bool cgi::CgiHandler::cgiProcessExited(const pid_t process_id, int* status) {
  pid_t re = waitpid(process_id, status, WNOHANG);
  // errorまたはprocessが終了していない
  // errorのときの処理はあやしい, -1のエラーはロジック的にありえない(process idがおかしい)
  if (re == 0) return false;
  // errorの時も子プロセスが存在しないと判断する
  return true;
}
