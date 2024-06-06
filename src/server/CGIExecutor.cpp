#include "CGIExecutor.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "ConfigHandler.hpp"
#include "LimitExcept.hpp"
#include "Utils.hpp"

const static char* kPhpExtension = ".php";
const static char* kPhp = "php";

cgi::CGIExecutor::CGIExecutor() {}

cgi::CGIExecutor::~CGIExecutor() {}

void cgi::CGIExecutor::executeCgiScript(const HttpRequest& request, const HttpResponse& response,
                                        const int cgi_sock, const int cli_sock) {
  std::string full_path = response.root_path_ + response.res_file_path_;
  prepareCgiExecution(request, response, full_path, cgi_sock, cli_sock);
  execve(this->script_path_.c_str(), const_cast<char* const*>(this->argv_.data()),
         const_cast<char* const*>(this->meta_vars_.data()));
  std::cerr << "webserv: [emerg] execve() failed (" << errno << ": " << std::strerror(errno) << ")"
            << std::endl;
  std::exit(EXIT_FAILURE);
}

void cgi::CGIExecutor::prepareCgiExecution(const HttpRequest& request, const HttpResponse& response,
                                           const std::string& full_path, const int cgi_sock, const int cli_sock) {
  if (!redirectStdIOToSocket(request, cgi_sock)) std::exit(EXIT_FAILURE);
  createScriptPath(full_path);
  createArgv(full_path);
  createMetaVars(request, response, cli_sock);
}

void cgi::CGIExecutor::createScriptPath(const std::string& script_path) {
  this->script_path_ = script_path;
  if (!Utils::isExtensionFile(script_path, kPhpExtension)) return;
  // スクリプトがphpの場合は、phpのpathを探す必要がある
  const std::string path = searchCommandInPath(kPhp);
  if (path == "") return;
  this->script_path_ = path;
}

void cgi::CGIExecutor::createArgv(const std::string& script_path) {
  if (Utils::isExtensionFile(script_path, kPhpExtension)) {
    this->argv_.push_back(kPhp);
    this->argv_.push_back(script_path.c_str());
    this->argv_.push_back(NULL);
    return;
  }
  const std::string::size_type n = script_path.rfind("/");
  const static std::string cgi_script = n == std::string::npos ? script_path : script_path.substr(n + 1);
  this->argv_.push_back(cgi_script.c_str());
  this->argv_.push_back(NULL);
}

void cgi::CGIExecutor::createMetaVars(const HttpRequest& request, const HttpResponse& response,
                                      const int cli_sock) {
  const static char* kContentType = "content-type";

  this->meta_vars_.push_back("AUTH_TYPE=");  // Authorizationをparseするロジックを実装しないため、値は空文字

  const static std::string content_length =
      std::string("CONTENT_LENGTH=") + Utils::toStr(request.body.size());
  this->meta_vars_.push_back(content_length.c_str());

  static std::string content_type = "CONTENT_TYPE=";
  if (request.headers.find(kContentType) != request.headers.end())
    content_type += request.headers.at(kContentType);
  this->meta_vars_.push_back(content_type.c_str());

  this->meta_vars_.push_back("GATEWAY_INTERFACE=CGI/1.1");

  static std::string path_info_var = "PATH_INFO=";
  if (!response.path_info_.empty()) path_info_var += response.path_info_;
  this->meta_vars_.push_back(path_info_var.c_str());

  this->meta_vars_.push_back(
      "PATH_TRANSLATED=");  // path translatedを受け渡す実装になっていないので値は空文字

  const static std::string query_string = std::string("QUERY_STRING=") + request.queries;
  this->meta_vars_.push_back(query_string.c_str());  // PATH_TRANSLATEDと同じ

  const std::string ip_address = Utils::socketToStrIPAddress(cli_sock);
  const static std::string remote_addr = std::string("REMOTE_ADDR=") + ip_address;
  this->meta_vars_.push_back(remote_addr.c_str());

  static std::string remote_host =
      std::string("REMOTE_HOST=") + ip_address;  // client host nameは取得できないので、ip address
  this->meta_vars_.push_back(remote_host.c_str());

  const static std::string method =
      std::string("REQUEST_METHOD=") + config::LimitExcept::MethodToStr(request.method);
  this->meta_vars_.push_back(method.c_str());

  const static std::string script_name = std::string("SCRIPT_NAME=") + response.res_file_path_;
  this->meta_vars_.push_back(script_name.c_str());

  const static std::string server_name = std::string("SERVER_NAME=") + request.headers.at("host");
  this->meta_vars_.push_back(server_name.c_str());

  const static std::string server_port =
      std::string("SERVER_PORT=") + Utils::toStr(Utils::resolveConnectedPort(cli_sock));
  this->meta_vars_.push_back(server_port.c_str());

  const static std::string server_protocol = std::string("SERVER_PROTOCOL=") + request.version;
  this->meta_vars_.push_back(server_protocol.c_str());

  this->meta_vars_.push_back("SERVER_SOFTWARE=webserv/1.0");

  this->meta_vars_.push_back(NULL);
}

std::vector<std::string> cgi::CGIExecutor::split(const std::string& s, char delimiter) const {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream is(s);
  while (std::getline(is, token, delimiter)) {
    token = token == "" ? "." : token;
    tokens.push_back(token);
  }
  return tokens;
}

bool cgi::CGIExecutor::isExecutableFile(const std::string& path) const {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) return false;
  return S_ISREG(statbuf.st_mode) && access(path.c_str(), X_OK) == 0;
}

std::string cgi::CGIExecutor::searchCommandInPath(const std::string& command) const {
  const char* path = std::getenv("PATH");
  if (path == NULL) return "";
  std::vector<std::string> directories = split(path, ':');
  for (size_t i = 0; i < directories.size(); ++i) {
    std::string command_path = directories[i] + "/" + command;
    if (isExecutableFile(command_path)) return command_path;
  }
  return "";
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
bool cgi::CGIExecutor::redirectStdIOToSocket(const HttpRequest& request, const int socket) const {
  if (dup2(socket, STDOUT_FILENO) == -1) {
    std::cerr << "webserv: [emerg] dup2() failed (" << errno << ": " << std::strerror(errno) << ")"
              << std::endl;
    close(socket);
    return false;
  }
  // bodyが存在する場合は、標準入力にbodyをセットする必要がある
  if (!request.body.empty()) {
    if (dup2(socket, STDIN_FILENO) == -1) {
      std::cerr << "webserv: [emerg] dup2() failed (" << errno << ": " << std::strerror(errno) << ")"
                << std::endl;
      close(STDOUT_FILENO);
      close(socket);
      return false;
    }
  }
  close(socket);
  return true;
}

const std::string& cgi::CGIExecutor::getScriptPath() const { return this->script_path_; }

const std::vector<const char*>& cgi::CGIExecutor::getArgv() const { return this->argv_; }

const std::vector<const char*>& cgi::CGIExecutor::getMetaVars() const { return this->meta_vars_; }
