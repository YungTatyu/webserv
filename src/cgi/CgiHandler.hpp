#ifndef CGI_CGI_HANDLER_HPP
#define CGI_CGI_HANDLER_HPP

#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "CgiExecutor.hpp"
#include "CgiParser.hpp"

namespace cgi {
enum CGI_SOCKET {
  SOCKET_PARENT = 0,
  SOCKET_CHILD = 1,
  SOCKET_READ = 0,
  SOCKET_WRITE = 1
};

class CgiHandler {
 private:
  CgiHandler(const CgiHandler&);
  CgiHandler& operator=(const CgiHandler&);
  bool forkCgiProcess(const HttpRequest& request, const HttpResponse& response);

  CgiParser cgi_parser_;
  CgiExecutor cgi_executor_;
  pid_t cgi_process_id_;
  int cli_socket_;  // cgiが紐づくクライアント

 public:
  CgiHandler();
  ~CgiHandler();
  static bool isCgi(const std::string& script_path);
  static bool cgiProcessExited(const pid_t process_id, int* status);
  bool callCgiExecutor(const HttpResponse& response, const HttpRequest& request, int cli_sock);
  bool callCgiParser(HttpResponse& response, const std::string& cgi_response);
  void killCgiProcess() const;
  const CgiParser& getCgiParser() const;
  const CgiExecutor& getCgiExecutor() const;
  pid_t getCgiProcessId() const;
  int getCliSocket() const;
  void setCliSocket(int socket);
  int getCgiSocket() const;
  void resetSockets();

  int sockets_[2];
};
}  // namespace cgi

#endif
