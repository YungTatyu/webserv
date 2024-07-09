#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <map>
#include <vector>

#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

struct TiedServer;
class CgiHandler;

class ConnectionData {
 public:
  enum EVENT {
    EV_READ = 0,
    EV_WRITE,
    EV_CGI_READ,
    EV_CGI_WRITE
  };

  ConnectionData() : sent_bytes_(0) {}
  ~ConnectionData() {}

  std::vector<unsigned char> raw_request_;
  std::vector<unsigned char> final_response_;
  std::vector<unsigned char> cgi_response_;
  size_t sent_bytes_;  // responseやcgi bodyをsendする際に送信したbyte数を記録する
  EVENT event_;
  HttpRequest request_;
  HttpResponse response_;
  cgi::CgiHandler cgi_handler_;
  const TiedServer* tied_server_;

 private:
  ConnectionData(const ConnectionData&);
  ConnectionData& operator=(const ConnectionData&);
};

typedef std::map<int, ConnectionData*>::size_type connection_size;

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager {
 public:
  ConnectionManager();
  ~ConnectionManager();
  bool setConnection(int fd);
  ConnectionData* getConnection(int fd);
  void setRawRequest(int fd, const std::vector<unsigned char>& raw_request);
  void addRawRequest(int fd, const std::vector<unsigned char>& raw_request, ssize_t read_bytes);
  bool setCgiConnection(int cli_sock, ConnectionData::EVENT event);
  void removeConnection(int fd, bool cgi);
  const std::vector<unsigned char>& getRawRequest(int fd) const;
  void setFinalResponse(int fd, const std::vector<unsigned char>& new_response);
  void addFinalResponse(int fd, const std::vector<unsigned char>& new_response);
  const std::vector<unsigned char>& getFinalResponse(int fd) const;
  void setEvent(int fd, ConnectionData::EVENT event);
  ConnectionData::EVENT getEvent(int fd) const;
  void setRequest(int fd, const HttpRequest& request);
  HttpRequest& getRequest(int fd);
  void setResponse(int fd, const HttpResponse& response);
  HttpResponse& getResponse(int fd);
  void addCgiResponse(int fd, const std::vector<unsigned char>& v, ssize_t read_bytes);
  const std::vector<unsigned char>& getCgiResponse(int fd) const;
  const std::map<int, ConnectionData*>& getConnections() const;
  void setTiedServer(int fd, const TiedServer* tied_server);
  const TiedServer& getTiedServer(int fd) const;
  cgi::CgiHandler& getCgiHandler(int fd) const;
  size_t getSentBytes(int fd) const;
  void addSentBytes(int fd, size_t bytes);
  void resetSentBytes(int fd);
  void resetCgiSockets(int fd);
  void clearRawRequest(int fd);
  void clearResData(int fd);
  void clearConnectionData(int fd);
  void closeAllConnections();
  bool isCgiSocket(int fd) const;
  void addClosedConnection(int fd);
  bool isClosedConnection(int fd) const;
  void clearClosedConnections();
  connection_size getCgiSockNum() const;

 private:
  ConnectionManager(const ConnectionManager&);
  ConnectionManager& operator=(const ConnectionManager&);

  std::map<int, ConnectionData*> connections_;
  std::set<int> closed_connections_;  // event handlerによって、接続を切られたfdを管理する
                                      // 発生したイベントのハンドラーが全て呼ばれたら毎回リセットされる
  connection_size cgi_sock_num_;
};

#endif
