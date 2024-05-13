#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <map>
#include <vector>

#include "CGIHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

struct TiedServer;
class CGIHandler;

class ConnectionData {
 public:
  enum EVENT {
    EV_READ,
    EV_WRITE,
    EV_CGI_READ,
    EV_CGI_WRITE,
  };
  std::vector<unsigned char>
      raw_request_;  // 画像などのテキスト以外のバイナリデータを扱う可能性があるのでstd::stringではなく、vector<char>にした。
  std::vector<unsigned char> final_response_;
  std::vector<unsigned char> cgi_response_;
  size_t sent_bytes_;  // responseやcgi bodyをsendする際に送信したbyte数を記録する
  EVENT event;
  HttpRequest request;
  HttpResponse response_;
  cgi::CGIHandler cgi_handler_;
  const TiedServer* tied_server_;
  ConnectionData() : sent_bytes_(0) {}
};

typedef std::map<int, ConnectionData*>::size_type connection_size;

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager {
 public:
  ConnectionManager();
  ~ConnectionManager();
  void setConnection(const int fd);
  ConnectionData* getConnection(const int fd);
  void addRawRequest(const int fd, const std::vector<unsigned char>& rawRequest, const ssize_t read_bytes);
  void setCgiConnection(const int cli_sock, const ConnectionData::EVENT event);
  void removeConnection(const int fd, const bool cgi);
  const std::vector<unsigned char>& getRawRequest(const int fd) const;
  void setFinalResponse(const int fd, const std::vector<unsigned char>& final_response);
  const std::vector<unsigned char>& getFinalResponse(const int fd) const;
  void setEvent(const int fd, const ConnectionData::EVENT event);
  ConnectionData::EVENT getEvent(const int fd) const;
  void setRequest(const int fd, const HttpRequest request);
  HttpRequest& getRequest(const int fd);
  void setResponse(const int fd, const HttpResponse response);
  HttpResponse& getResponse(const int fd);
  void addCgiResponse(const int fd, const std::vector<unsigned char>& v, const ssize_t read_bytes);
  const std::vector<unsigned char>& getCgiResponse(const int fd) const;
  const std::map<int, ConnectionData*>& getConnections() const;
  void setTiedServer(const int fd, const TiedServer* tied_server);
  const TiedServer& getTiedServer(const int fd) const;
  const cgi::CGIHandler& getCgiHandler(const int fd) const;
  size_t getSentBytes(const int fd) const;
  void addSentBytes(const int fd, const size_t bytes);
  bool callCgiExecutor(const int fd, const std::string& script_path, const HttpRequest& request);
  bool callCgiParser(const int fd, HttpResponse& response, const std::string& cgi_response);
  void resetSentBytes(const int fd);
  void resetCgiSockets(const int fd);
  void clearRawRequest(const int fd);
  void clearConnectionData(const int fd);
  void closeAllConnections();
  bool isCgiSocket(const int fd) const;
  connection_size getCgiSockNum() const;

 private:
  std::map<int, ConnectionData*> connections_;
  connection_size cgi_sock_num_;
};

#endif
