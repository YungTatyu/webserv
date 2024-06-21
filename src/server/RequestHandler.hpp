#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "ConnectionManager.hpp"
#include "IServer.hpp"
#include "NetworkIOHandler.hpp"
#include "TimerTree.hpp"

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler {
 public:
  /**
   * >0 の値はfdと被るので避ける
   * -1 はシステムコールのエラーと被るので避ける
   */
  enum UPDATE_STATUS {
    UPDATE_NONE = -10,
    UPDATE_READ = -11,
    UPDATE_WRITE = -12,
    UPDATE_CGI_READ = -13,
    UPDATE_CGI_WRITE = -14,
    UPDATE_CLOSE = -15
  };
  RequestHandler();
  int handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                      TimerTree &timerTree, const int sockfd) const;
  int handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                       TimerTree &timerTree, const int sockfd) const;
  int handleCgiReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                         TimerTree &timerTree, const int sockfd) const;
  int handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                          TimerTree &timerTree, const int sockfd) const;
  int handleEofEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                     TimerTree &timerTree, const int sockfd) const;
  int handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, IServer *server,
                       TimerTree &timerTree, const int sockfd) const;
  std::map<int, RequestHandler::UPDATE_STATUS> handleTimeoutEvent(NetworkIOHandler &ioHandler,
                                                                  ConnectionManager &connManager,
                                                                  IServer *server,
                                                                  TimerTree &timerTree) const;

 private:
  bool cgiProcessExited(const pid_t process_id, int &status) const;
  int handleRequest(ConnectionManager &connManager, const ConfigHandler &configHandler, IServer *server,
                    TimerTree &timerTree, const int sockfd) const;
  int handleResponse(ConnectionManager &connManager, const ConfigHandler &configHandler, IServer *server,
                     TimerTree &timerTree, const int sockfd) const;
  int handleCgi(ConnectionManager &connManager, const ConfigHandler &configHandler, IServer *server,
                TimerTree &timerTree, const int sockfd) const;
  void addTimerByType(ConnectionManager &connManager, const ConfigHandler &configHandler,
                      TimerTree &timerTree, const int sockfd, enum Timer::TimeoutType type) const;
  bool isOverWorkerConnections(ConnectionManager &connManager, const ConfigHandler &configHandler) const;
};

#endif
