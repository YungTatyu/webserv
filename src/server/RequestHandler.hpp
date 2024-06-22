#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "ConnectionManager.hpp"
#include "IServer.hpp"
#include "NetworkIOHandler.hpp"
#include "TimerTree.hpp"

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler {
 public:
  RequestHandler();
  void handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                       TimerTree &timerTree, const int sockfd) const;
  void handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                        TimerTree &timerTree, const int sockfd) const;
  void handleCgiReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                          TimerTree &timerTree, const int sockfd) const;
  void handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                           TimerTree &timerTree, const int sockfd) const;
  void handleEofEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                      TimerTree &timerTree, const int sockfd) const;
  void handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                        TimerTree &timerTree, const int sockfd) const;
  void handleTimeoutEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                          TimerTree &timerTree) const;

 private:
  bool cgiProcessExited(const pid_t process_id, int &status) const;
  void handleRequest(ConnectionManager &connManager, const ConfigHandler &configHandler, IServer *server,
                     TimerTree &timerTree, const int sockfd) const;
  void handleResponse(ConnectionManager &connManager, const ConfigHandler &configHandler, IServer *server,
                      TimerTree &timerTree, const int sockfd) const;
  void handleCgi(ConnectionManager &connManager, const ConfigHandler &configHandler, IServer *server,
                 TimerTree &timerTree, const int sockfd) const;
  void addTimerByType(ConnectionManager &connManager, const ConfigHandler &configHandler,
                      TimerTree &timerTree, const int sockfd, enum Timer::TimeoutType type) const;
  bool isOverWorkerConnections(ConnectionManager &connManager, const ConfigHandler &configHandler) const;
};

#endif
