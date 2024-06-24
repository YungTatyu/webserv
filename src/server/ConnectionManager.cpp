#include "ConnectionManager.hpp"

#include <unistd.h>

#include <algorithm>

ConnectionManager::ConnectionManager() : cgi_sock_num_(0) {}

ConnectionManager::~ConnectionManager() { closeAllConnections(); }

/**
 * @brief client or listen socketを登録する
 *
 * @param fd
 */
void ConnectionManager::setConnection(const int fd) {
  connections_[fd] = new ConnectionData();
  std::cout << "new connection:" << fd << "\n";
}

/**
 * @brief cgiのsocketを登録する
 *
 * @param cli_sock cgiのsocketに紐づいているクライアントのソケット
 * @param event
 */
void ConnectionManager::setCgiConnection(const int cli_sock, const ConnectionData::EVENT event) {
  ConnectionData* cd = this->connections_.at(cli_sock);
  const int cgi_sock = cd->cgi_handler_.getCgiSocket();
  this->connections_.insert(std::make_pair(cgi_sock, cd));
  // cgi のイベントに更新
  this->connections_.at(cli_sock)->event = event;
  this->cgi_sock_num_++;
}

/**
 * @brief connection mapから削除
 * cgi: connection dataを削除しない
 *      clientがデータを必要とするため
 *      cgi_sock_num_をdecrement
 *
 * @param fd
 * @param cgi
 */
void ConnectionManager::removeConnection(const int fd, const bool cgi) {
  if (!cgi) {
    std::cerr << "delete connection:" << fd << "\n";
    delete connections_.at(fd);
  } else
    this->cgi_sock_num_--;
  connections_.erase(fd);
  closed_connections_.insert(fd);
}

ConnectionData* ConnectionManager::getConnection(const int fd) { return connections_.at(fd); }

void ConnectionManager::setRawRequest(const int fd, const std::vector<unsigned char>& rawRequest) {
  connections_[fd]->raw_request_ = rawRequest;
}

void ConnectionManager::addRawRequest(const int fd, const std::vector<unsigned char>& rawRequest,
                                      const ssize_t read_bytes) {
  connections_[fd]->raw_request_.insert(connections_[fd]->raw_request_.end(), rawRequest.begin(),
                                        rawRequest.begin() + read_bytes);
}

const std::vector<unsigned char>& ConnectionManager::getRawRequest(const int fd) const {
  return connections_.at(fd)->raw_request_;
}

void ConnectionManager::setFinalResponse(const int fd, const std::vector<unsigned char>& new_response) {
  connections_.at(fd)->final_response_ = new_response;
}

void ConnectionManager::addFinalResponse(const int fd, const std::vector<unsigned char>& new_response) {
  std::vector<unsigned char>& final_response = connections_.at(fd)->final_response_;
  final_response.insert(final_response.end(), new_response.begin(), new_response.end());
}

const std::vector<unsigned char>& ConnectionManager::getFinalResponse(const int fd) const {
  return connections_.at(fd)->final_response_;
}

/**
 *
 * 監視するイベントを更新
 * イベントをupdateする際にも使用
 *
 */
void ConnectionManager::setEvent(const int fd, const ConnectionData::EVENT event) {
  connections_[fd]->event = event;
}

ConnectionData::EVENT ConnectionManager::getEvent(const int fd) const { return connections_.at(fd)->event; }

const std::map<int, ConnectionData*>& ConnectionManager::getConnections() const { return this->connections_; }

void ConnectionManager::setRequest(const int fd, const HttpRequest request) {
  connections_[fd]->request = request;
}

HttpRequest& ConnectionManager::getRequest(const int fd) { return connections_.at(fd)->request; }

void ConnectionManager::setResponse(const int fd, const HttpResponse response) {
  connections_[fd]->response_ = response;
}

HttpResponse& ConnectionManager::getResponse(const int fd) { return connections_.at(fd)->response_; }

const std::vector<unsigned char>& ConnectionManager::getCgiResponse(const int fd) const {
  return this->connections_.at(fd)->cgi_response_;
}

bool ConnectionManager::callCgiExecutor(const int fd, const HttpResponse& response,
                                        const HttpRequest& request) {
  return this->connections_.at(fd)->cgi_handler_.callCgiExecutor(response, request, fd);
}

bool ConnectionManager::callCgiParser(const int fd, HttpResponse& response, const std::string& cgi_response) {
  return this->connections_.at(fd)->cgi_handler_.callCgiParser(response, cgi_response);
}

void ConnectionManager::addCgiResponse(const int fd, const std::vector<unsigned char>& v,
                                       const ssize_t read_bytes) {
  connections_[fd]->cgi_response_.insert(connections_[fd]->cgi_response_.end(), v.begin(),
                                         v.begin() + read_bytes);
}

void ConnectionManager::setTiedServer(const int fd, const TiedServer* tied_server) {
  connections_[fd]->tied_server_ = tied_server;
}

const TiedServer& ConnectionManager::getTiedServer(const int fd) const {
  return *(connections_.at(fd)->tied_server_);
}

const cgi::CgiHandler& ConnectionManager::getCgiHandler(const int fd) const {
  return connections_.at(fd)->cgi_handler_;
}

size_t ConnectionManager::getSentBytes(const int fd) const { return connections_.at(fd)->sent_bytes_; }

void ConnectionManager::addSentBytes(const int fd, const size_t bytes) {
  connections_.at(fd)->sent_bytes_ += bytes;
}

void ConnectionManager::resetSentBytes(const int fd) { connections_.at(fd)->sent_bytes_ = 0; }

void ConnectionManager::resetCgiSockets(const int fd) { connections_.at(fd)->cgi_handler_.resetSockets(); }

void ConnectionManager::clearRawRequest(const int fd) {
  ConnectionData* cd = this->connections_.at(fd);
  cd->raw_request_.clear();
}

/**
 * @brief リクエストデータ以外を削除する
 * 一人のクライアントからの複数のリクエストをさばく時に呼ぶ
 */
void ConnectionManager::clearResData(const int fd) {
  ConnectionData* cd = this->connections_.at(fd);
  cd->final_response_.clear();
  cd->cgi_response_.clear();
  resetCgiSockets(fd);
  resetSentBytes(fd);
}

void ConnectionManager::clearConnectionData(const int fd) {
  clearRawRequest(fd);
  clearResData(fd);
}

bool ConnectionManager::isCgiSocket(const int fd) const {
  const int cgi_sock = this->connections_.at(fd)->cgi_handler_.getCgiSocket();
  return fd == cgi_sock;
}

void ConnectionManager::addClosedConnection(int fd) { this->closed_connections_.insert(fd); }

/**
 * @ brief event handlerによって、閉じれたconectionかを確認する
 */
bool ConnectionManager::isClosedConnection(int fd) const {
  return this->closed_connections_.find(fd) != this->closed_connections_.end();
}

void ConnectionManager::clearClosedConnections() { this->closed_connections_.clear(); }

void ConnectionManager::closeAllConnections() {
  for (std::map<int, ConnectionData*>::iterator it = this->connections_.begin();
       it != this->connections_.end();) {
    std::map<int, ConnectionData*>::iterator tmp = it;
    ++it;
    if (isCgiSocket(tmp->first)) {
      close(tmp->first);
      connections_.erase(tmp->first);
    }
  }
  for (std::map<int, ConnectionData*>::iterator it = this->connections_.begin();
       it != this->connections_.end(); ++it) {
    close(it->first);
    delete it->second;
  }
}

connection_size ConnectionManager::getCgiSockNum() const { return cgi_sock_num_; }
