#include "PollServer.hpp"

PollServer::PollServer() {}

PollServer::~PollServer() {}

void PollServer::eventLoop(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                           NetworkIOHandler* io_handler, RequestHandler* request_handler,
                           ConfigHandler* config_handler, TimerTree* timer_tree) {
  for (;;) {
    waitForEvent(conn_manager, event_manager, timer_tree);

    // 発生したイベントをhandleする
    callEventHandler(conn_manager, event_manager, io_handler, request_handler, config_handler, timer_tree);

    // 発生したすべてのイベントを削除
    event_manager->clearAllEvents();
  }
}

int PollServer::waitForEvent(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                             TimerTree* timer_tree) {
  std::vector<pollfd> pollfds = convertToPollfds(*conn_manager);

  // 現在時刻を更新
  Timer::updateCurrentTime();

  // TODO: error起きたときどうしようか? 一定数retry? serverはdownしたらダメな気がする
  int re = poll(pollfds.data(), pollfds.size(), timer_tree->findTimer());

  // 発生したイベントをActiveEventManagerにすべて追加
  addActiveEvents(pollfds, event_manager);
  return re;
}

void PollServer::addActiveEvents(const std::vector<pollfd>& pollfds, IActiveEventManager* event_manager) {
  for (size_t i = 0; i < pollfds.size(); ++i) {
    const struct pollfd& cur_pfd = pollfds[i];
    // イベントが発生していたら、active_eventに追加
    if (event_manager->isReadEvent(static_cast<const void*>(&cur_pfd)) ||
        event_manager->isWriteEvent(static_cast<const void*>(&cur_pfd)) ||
        event_manager->isErrorEvent(static_cast<const void*>(&cur_pfd)))
      event_manager->addEvent(static_cast<const void*>(&cur_pfd));
  }
}

void PollServer::callEventHandler(ConnectionManager* conn_manager, IActiveEventManager* event_manager,
                                  NetworkIOHandler* io_handler, RequestHandler* request_handler,
                                  ConfigHandler* config_handler, TimerTree* timer_tree) {
  const std::vector<pollfd>* active_events =
      static_cast<const std::vector<pollfd>*>(event_manager->getActiveEvents());

  // TimeoutEvent発生
  if (event_manager->getActiveEventsNum() == 0) {
    request_handler->handleTimeoutEvent(*io_handler, *conn_manager, *config_handler, *timer_tree);
    return;
  }

  // 現在時刻を更新
  Timer::updateCurrentTime();

  // 発生したイベントの数だけloopする
  for (std::vector<pollfd>::const_iterator it = active_events->begin(); it != active_events->end(); ++it) {
    // 発生したeventに対するhandlerを呼ぶ
    // interfaceを実装したことにより、関数ポインタのmapが使えなくなった・・・　どうしよう？？？
    if (event_manager->isReadEvent(static_cast<const void*>(&(*it))))
      request_handler->handleReadEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, it->fd);
    else if (event_manager->isWriteEvent(static_cast<const void*>(&(*it))))
      request_handler->handleWriteEvent(*io_handler, *conn_manager, *config_handler, *timer_tree, it->fd);
    else if (event_manager->isEofEvent(static_cast<const void*>(&(*it))))
      request_handler->handleEofEvent(*io_handler, *conn_manager, *timer_tree, it->fd);
    else if (event_manager->isErrorEvent(static_cast<const void*>(&(*it))))
      request_handler->handleErrorEvent(*io_handler, *conn_manager, *timer_tree, it->fd);
  }
}

/**
 * @brief pollfdのvectorを作成する
 *
 * @param connections : すべてのクライアントソケットとそれにひもづくデータ
 * @return std::vector<struct pollfd>
 */
std::vector<struct pollfd> PollServer::convertToPollfds(const ConnectionManager& conn_manager) {
  std::vector<struct pollfd> list;
  const std::map<int, ConnectionData*>& connections = conn_manager.getConnections();
  for (std::map<int, ConnectionData*>::const_iterator it = connections.begin(); it != connections.end();
       ++it) {
    struct pollfd pollfd;
    switch (it->second->event) {
      case ConnectionData::EV_CGI_READ:
      case ConnectionData::EV_CGI_WRITE:
        // cgi eventの時は、クライアントsocketはイベント登録しない
        if (!conn_manager.isCgiSocket(it->first)) continue;
        break;
      default:
        break;
    }
    pollfd.fd = it->first;
    switch (it->second->event) {
      case ConnectionData::EV_READ:
      case ConnectionData::EV_CGI_READ:
        pollfd.events = POLLIN;
        break;
      case ConnectionData::EV_WRITE:
      case ConnectionData::EV_CGI_WRITE:
        pollfd.events = POLLOUT;
        break;
    }
    pollfd.revents = 0;
    list.push_back(pollfd);
  }
  return list;
}
