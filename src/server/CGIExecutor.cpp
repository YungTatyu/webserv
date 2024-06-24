#include "CgiExecutor.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <sstream>

#include "ConfigHandler.hpp"
#include "LimitExcept.hpp"
#include "SysCallWrapper.hpp"
#include "Utils.hpp"
#include "WebServer.hpp"
#include "error.hpp"

cgi::CgiExecutor::CgiExecutor() {}

cgi::CgiExecutor::~CgiExecutor() {}

void cgi::CgiExecutor::executeCgiScript(const HttpRequest& request, const HttpResponse& response,
                                         int cgi_sock,  int cli_sock) {
  std::string full_path = response.root_path_ + response.res_file_path_;
  prepareCgiExecution(request, response, full_path, cgi_sock, cli_sock);
  execve(this->script_path_.c_str(), const_cast<char* const*>(this->argv_.data()),
         const_cast<char* const*>(this->meta_vars_.data()));
  WebServer::writeErrorlog(error::strSysCallError("execve") + "\n");
  std::exit(EXIT_FAILURE);
}

void cgi::CgiExecutor::prepareCgiExecution(const HttpRequest& request, const HttpResponse& response,
                                           const std::string& full_path,  int cgi_sock,
                                            int cli_sock) {
  if (!redirectStdIOToSocket(request, cgi_sock)) std::exit(EXIT_FAILURE);
  this->script_path_ = full_path;
  createArgv(full_path);
  createMetaVars(request, response, cli_sock);
}

void cgi::CgiExecutor::createArgv(const std::string& script_path) {
  const std::string::size_type n = script_path.rfind("/");
  const std::string cgi_script = n == std::string::npos ? script_path : script_path.substr(n + 1);
  this->argv_.push_back(strdup_from_string(cgi_script));
  this->argv_.push_back(NULL);
}

void cgi::CgiExecutor::createMetaVars(const HttpRequest& request, const HttpResponse& response,
                                       int cli_sock) {
  const static char* kContentType = "content-type";

  this->meta_vars_.push_back("AUTH_TYPE=");  // Authorizationをparseするロジックを実装しないため、値は空文字

  const std::string content_length = std::string("CONTENT_LENGTH=") + Utils::toStr(request.body.size());
  this->meta_vars_.push_back(strdup_from_string(content_length));

  std::string content_type = "CONTENT_TYPE=";
  if (request.headers.find(kContentType) != request.headers.end())
    content_type += request.headers.at(kContentType);
  this->meta_vars_.push_back(strdup_from_string(content_type));

  this->meta_vars_.push_back("GATEWAY_INTERFACE=CGI/1.1");

  std::string path_info_var = "PATH_INFO=";
  if (!response.path_info_.empty()) path_info_var += response.path_info_;
  this->meta_vars_.push_back(strdup_from_string(path_info_var));

  this->meta_vars_.push_back(
      "PATH_TRANSLATED=");  // path translatedを受け渡す実装になっていないので値は空文字

  const std::string query_string = std::string("QUERY_STRING=") + request.queries;
  this->meta_vars_.push_back(strdup_from_string(query_string));  // PATH_TRANSLATEDと同じ

  const std::string ip_address = Utils::socketToStrIPAddress(cli_sock);
  const std::string remote_addr = std::string("REMOTE_ADDR=") + ip_address;
  this->meta_vars_.push_back(strdup_from_string(remote_addr));

  std::string remote_host =
      std::string("REMOTE_HOST=") + ip_address;  // client host nameは取得できないので、ip address
  this->meta_vars_.push_back(strdup_from_string(remote_host));

  const std::string method =
      std::string("REQUEST_METHOD=") + config::LimitExcept::MethodToStr(request.method);
  this->meta_vars_.push_back(strdup_from_string(method));

  const std::string script_name = std::string("SCRIPT_NAME=") + response.res_file_path_;
  this->meta_vars_.push_back(strdup_from_string(script_name));

  const std::string server_name = std::string("SERVER_NAME=") + request.headers.at("host");
  this->meta_vars_.push_back(strdup_from_string(server_name));

  const std::string server_port =
      std::string("SERVER_PORT=") + Utils::toStr(Utils::resolveConnectedPort(cli_sock));
  this->meta_vars_.push_back(strdup_from_string(server_port));

  const std::string server_protocol = std::string("SERVER_PROTOCOL=") + request.version;
  this->meta_vars_.push_back(strdup_from_string(server_protocol));

  this->meta_vars_.push_back("SERVER_SOFTWARE=webserv/1.0");

  for (std::map<std::string, std::string, Utils::CaseInsensitiveCompare>::const_iterator it =
           request.headers.begin();
       it != request.headers.end(); ++it) {
    if (Utils::compareIgnoreCase(it->first, kContentType) ||
        Utils::compareIgnoreCase(it->first, "content-length"))
      continue;
    std::string meta_var = "HTTP_";
    meta_var = meta_var + Utils::replace(Utils::toUpper(it->first), '-', '_') + "=" + it->second;
    // hostの場合はrequestをraw dataのhostヘッダーのvalueとして渡さないといけない（portの情報を復活させる）
    if (Utils::compareIgnoreCase(it->first, "host")) meta_var += request.port_in_host;
    this->meta_vars_.push_back(strdup_from_string(meta_var));
  }

  this->meta_vars_.push_back(NULL);
}

std::vector<std::string> cgi::CgiExecutor::split(const std::string& s, char delimiter) const {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream is(s);
  while (std::getline(is, token, delimiter)) {
    token = token == "" ? "." : token;
    tokens.push_back(token);
  }
  return tokens;
}

bool cgi::CgiExecutor::isExecutableFile(const std::string& path) const {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) return false;
  return S_ISREG(statbuf.st_mode) && access(path.c_str(), X_OK) == 0;
}

/**
 * @brief socketを標準入力と標準出力に複製する
 *
 * 標準入力からはbodyを受け取る
 * 標準出力はcgiスクリプトのresponseを書き込む
 *
 * @param socket
 * @return true
 * @return false
 */
bool cgi::CgiExecutor::redirectStdIOToSocket(const HttpRequest& request,  int socket) const {
  if (SysCallWrapper::Dup2(socket, STDOUT_FILENO) == -1) {
    close(socket);
    return false;
  }
  // bodyが存在する場合は、標準入力にbodyをセットする必要がある
  if (!request.body.empty()) {
    if (SysCallWrapper::Dup2(socket, STDIN_FILENO) == -1) {
      close(STDOUT_FILENO);
      close(socket);
      return false;
    }
  }
  close(socket);
  return true;
}

char* cgi::CgiExecutor::strdup_from_string(const std::string& str) const {
  size_t size = str.size() + 1;
  char* new_cstr = new char[size];
  std::memcpy(new_cstr, str.c_str(), sizeof(char) * size);
  return new_cstr;
}

const std::string& cgi::CgiExecutor::getScriptPath() const { return this->script_path_; }

const std::vector<const char*>& cgi::CgiExecutor::getArgv() const { return this->argv_; }

const std::vector<const char*>& cgi::CgiExecutor::getMetaVars() const { return this->meta_vars_; }
