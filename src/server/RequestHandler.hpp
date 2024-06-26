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
  void handleReadEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                       TimerTree &timer_tree, int sock) const;
  void handleWriteEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                        TimerTree &timer_tree, int sock) const;
  void handleCgiReadEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                          TimerTree &timer_tree, int sock) const;
  void handleCgiWriteEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                           TimerTree &timer_tree, int sock) const;
  void handleEofEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                      TimerTree &timer_tree, int sock) const;
  void handleErrorEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                        TimerTree &timer_tree, int sock) const;
  void handleTimeoutEvent(NetworkIOHandler &io_handler, ConnectionManager &conn_manager, IServer *server,
                          TimerTree &timer_tree) const;

 private:
  bool cgiProcessExited(pid_t process_id, int &status) const;
  void handleRequest(ConnectionManager &conn_manager, const ConfigHandler &config_handler, IServer *server,
                     TimerTree &timer_tree, int sock) const;
  void handleResponse(ConnectionManager &conn_manager, const ConfigHandler &config_handler, IServer *server,
                      TimerTree &timer_tree, int sock) const;
  void handleCgi(ConnectionManager &conn_manager, const ConfigHandler &config_handler, IServer *server,
                 TimerTree &timer_tree, int sock) const;
  void addTimerByType(ConnectionManager &conn_manager, const ConfigHandler &config_handler,
                      TimerTree &timer_tree, int sock, enum Timer::TimeoutType type) const;
  bool isOverWorkerConnections(ConnectionManager &conn_manager, const ConfigHandler &config_handler) const;
};

#endif
