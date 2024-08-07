#include "CgiExecutor.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <sstream>

#include "ConfigHandler.hpp"
#include "LimitExcept.hpp"
#include "WebServer.hpp"
#include "error.hpp"
#include "syscall_wrapper.hpp"
#include "utils.hpp"

cgi::CgiExecutor::CgiExecutor() {}

cgi::CgiExecutor::CgiExecutor(const CgiExecutor& other) { *this = other; }

cgi::CgiExecutor::~CgiExecutor() {}

cgi::CgiExecutor& cgi::CgiExecutor::operator=(const CgiExecutor& other) {
  if (this != &other) {
    this->script_path_ = other.script_path_;
    this->argv_ = other.argv_;
    this->meta_vars_ = other.meta_vars_;
  }
  return *this;
}

void cgi::CgiExecutor::executeCgiScript(const HttpRequest& request, const HttpResponse& response,
                                        int cgi_sock, int cli_sock) {
  std::string full_path = response.root_path_ + response.res_file_path_;
  prepareCgiExecution(request, response, full_path, cgi_sock, cli_sock);
  execve(this->script_path_.c_str(), const_cast<char* const*>(this->argv_.data()),
         const_cast<char* const*>(this->meta_vars_.data()));
  WebServer::writeErrorlog(error::strSysCallError("execve"), config::WARN);
  std::exit(EXIT_FAILURE);
}

void cgi::CgiExecutor::prepareCgiExecution(const HttpRequest& request, const HttpResponse& response,
                                           const std::string& full_path, int cgi_sock, int cli_sock) {
  if (!redirectStdIOToSocket(request, cgi_sock)) std::exit(EXIT_FAILURE);
  createArgv(full_path);
  createMetaVars(request, response, cli_sock);
  // scriptが存在するdirに移動する
  if (syscall_wrapper::Chdir(createCgiDirPath(full_path).c_str()) == -1) std::exit(EXIT_FAILURE);
}

/**
 * @brief script pathとargvを生成する
 * scriptのpathを指定するが、pathは固定で、["./" + script_name]
 * なぜなら、execveでスクリプトを実行する前に、chdir()でcur dirをscriptのpathに変更するから
 */
void cgi::CgiExecutor::createArgv(const std::string& full_path) {
  std::string::size_type n = full_path.rfind("/");
  std::string script_name = n == std::string::npos ? full_path : full_path.substr(n + 1);
  this->script_path_ = "./" + script_name;
  this->argv_.push_back(strdupFromString(script_name));
  this->argv_.push_back(NULL);
}

void cgi::CgiExecutor::createMetaVars(const HttpRequest& request, const HttpResponse& response,
                                      int cli_sock) {
  static const char* kContentType = "content-type";

  this->meta_vars_.push_back("AUTH_TYPE=");  // Authorizationをparseするロジックを実装しないため、値は空文字

  const std::string content_length = std::string("CONTENT_LENGTH=") + utils::toStr(request.body_.size());
  this->meta_vars_.push_back(strdupFromString(content_length));

  std::string content_type = "CONTENT_TYPE=";
  if (request.headers_.find(kContentType) != request.headers_.end())
    content_type += request.headers_.at(kContentType);
  this->meta_vars_.push_back(strdupFromString(content_type));

  this->meta_vars_.push_back("GATEWAY_INTERFACE=CGI/1.1");

  std::string path_info_var = "PATH_INFO=";
  if (!response.path_info_.empty()) path_info_var += response.path_info_;
  this->meta_vars_.push_back(strdupFromString(path_info_var));

  this->meta_vars_.push_back(
      "PATH_TRANSLATED=");  // path translatedを受け渡す実装になっていないので値は空文字

  const std::string query_string = std::string("QUERY_STRING=") + request.queries_;
  this->meta_vars_.push_back(strdupFromString(query_string));  // PATH_TRANSLATEDと同じ

  const std::string ip_address = utils::socketToStrIPAddress(cli_sock);
  const std::string remote_addr = std::string("REMOTE_ADDR=") + ip_address;
  this->meta_vars_.push_back(strdupFromString(remote_addr));

  std::string remote_host =
      std::string("REMOTE_HOST=") + ip_address;  // client host nameは取得できないので、ip address
  this->meta_vars_.push_back(strdupFromString(remote_host));

  const std::string method =
      std::string("REQUEST_METHOD=") + config::LimitExcept::MethodToStr(request.method_);
  this->meta_vars_.push_back(strdupFromString(method));

  const std::string script_name = std::string("SCRIPT_NAME=") + response.res_file_path_;
  this->meta_vars_.push_back(strdupFromString(script_name));

  const std::string server_name = std::string("SERVER_NAME=") + request.headers_.at("host");
  this->meta_vars_.push_back(strdupFromString(server_name));

  const std::string server_port =
      std::string("SERVER_PORT=") + utils::toStr(utils::resolveConnectedPort(cli_sock));
  this->meta_vars_.push_back(strdupFromString(server_port));

  const std::string server_protocol = std::string("SERVER_PROTOCOL=") + request.version_;
  this->meta_vars_.push_back(strdupFromString(server_protocol));

  this->meta_vars_.push_back("SERVER_SOFTWARE=webserv/1.0");

  for (std::map<std::string, std::string, utils::CaseInsensitiveCompare>::const_iterator it =
           request.headers_.begin();
       it != request.headers_.end(); ++it) {
    if (utils::compareIgnoreCase(it->first, kContentType) ||
        utils::compareIgnoreCase(it->first, "content-length"))
      continue;
    std::string meta_var = "HTTP_";
    meta_var = meta_var + utils::replace(utils::toUpper(it->first), '-', '_') + "=" + it->second;
    // hostの場合はrequestをraw dataのhostヘッダーのvalueとして渡さないといけない（portの情報を復活させる）
    if (utils::compareIgnoreCase(it->first, "host")) meta_var += request.port_in_host_;
    this->meta_vars_.push_back(strdupFromString(meta_var));
  }

  this->meta_vars_.push_back(NULL);
}

/**
 * '/'が見つからない場合は、文字列そのままを返す
 */
std::string cgi::CgiExecutor::createCgiDirPath(const std::string& script_path) const {
  return script_path.substr(0, script_path.rfind("/"));
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
bool cgi::CgiExecutor::redirectStdIOToSocket(const HttpRequest& request, int socket) const {
  if (syscall_wrapper::Dup2(socket, STDOUT_FILENO) == -1) {
    close(socket);
    return false;
  }
  // bodyが存在する場合は、標準入力にbodyをセットする必要がある
  if (!request.body_.empty()) {
    if (syscall_wrapper::Dup2(socket, STDIN_FILENO) == -1) {
      close(STDOUT_FILENO);
      close(socket);
      return false;
    }
  }
  close(socket);
  return true;
}

char* cgi::CgiExecutor::strdupFromString(const std::string& str) const {
  size_t size = str.size() + 1;
  char* new_cstr = new char[size];
  std::memcpy(new_cstr, str.c_str(), sizeof(char) * size);
  return new_cstr;
}

const std::string& cgi::CgiExecutor::getScriptPath() const { return this->script_path_; }

const std::vector<const char*>& cgi::CgiExecutor::getArgv() const { return this->argv_; }

const std::vector<const char*>& cgi::CgiExecutor::getMetaVars() const { return this->meta_vars_; }
