#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include "ConnectionManager.hpp"
#include "IServer.hpp"
#include "NetworkIOHandler.hpp"
#include "TimerTree.hpp"

class EventHandler {
 public:
  EventHandler();
  ~EventHandler();
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
  EventHandler(const EventHandler &);
  EventHandler &operator=(const EventHandler &);
  bool cgiProcessExited(pid_t process_id, int &status) const;
  void handleRequest(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                     const ConfigHandler &config_handler, IServer *server, TimerTree &timer_tree,
                     int sock) const;
  void handleResponse(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                      const ConfigHandler &config_handler, IServer *server, TimerTree &timer_tree,
                      int sock) const;
  void handleCgi(NetworkIOHandler &io_handler, ConnectionManager &conn_manager,
                 const ConfigHandler &config_handler, IServer *server, TimerTree &timer_tree, int sock) const;
  void addTimerByType(ConnectionManager &conn_manager, const ConfigHandler &config_handler,
                      TimerTree &timer_tree, int sock, enum Timer::TimeoutType type) const;
  bool isOverWorkerConnections(ConnectionManager &conn_manager, const ConfigHandler &config_handler) const;
  void waitKilledProcess(std::vector<pid_t> &killed_pids) const;
};

#endif
