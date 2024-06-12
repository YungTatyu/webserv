#ifndef CGI_CGI_EXECUTOR_HPP
#define CGI_CGI_EXECUTOR_HPP

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

namespace cgi {
class CGIExecutor {
 private:
  std::string script_path_;
  std::vector<const char*> argv_;
  std::vector<const char*> meta_vars_;  // メタ変数(環境変数)
  void prepareCgiExecution(const HttpRequest& request, const HttpResponse& response,
                           const std::string& full_path, const int cgi_sock, const int cli_sock);
  void createArgv(const std::string& script_path);
  void createMetaVars(const HttpRequest& request, const HttpResponse& response, const int cli_sock);
  std::vector<std::string> split(const std::string& s, char delimiter) const;
  bool isExecutableFile(const std::string& path) const;

 public:
  CGIExecutor();
  ~CGIExecutor();
  void executeCgiScript(const HttpRequest& request, const HttpResponse& response, const int cgi_sock,
                        const int cli_sock);
  const std::string& getScriptPath() const;
  const std::vector<const char*>& getArgv() const;
  const std::vector<const char*>& getMetaVars() const;
  bool redirectStdIOToSocket(const HttpRequest& request, const int socket) const;
};
}  // namespace cgi

#endif
