#include "PollActiveEventManager.hpp"

#include <poll.h>

PollActiveEventManager::PollActiveEventManager() {}

PollActiveEventManager::PollActiveEventManager(const PollActiveEventManager &other) { *this = other; }

PollActiveEventManager::~PollActiveEventManager() {}

PollActiveEventManager &PollActiveEventManager::operator=(const PollActiveEventManager &other) {
  if (this != &other) {
    this->active_events_ = other.active_events_;
    this->active_events_num_ = other.active_events_num_;
  }
  return *this;
}

void PollActiveEventManager::addEvent(const void *event) {
  const pollfd *poll_fd = static_cast<const pollfd *>(event);
  this->active_events_.push_back(*poll_fd);
}

void *PollActiveEventManager::getActiveEvents() { return static_cast<void *>(&(this->active_events_)); }

int PollActiveEventManager::getActiveEventsNum() { return this->active_events_.size(); }

void PollActiveEventManager::setActiveEventsNum(int num) { this->active_events_num_ = num; }

void PollActiveEventManager::clearAllEvents() { this->active_events_.clear(); }

/**
 * @brief メモリが不足した場合に、active_events_が確保した余分なメモリを解放するためのメソッド
 */
void PollActiveEventManager::reallocActiveEvents(std::size_t size) {
  if (this->active_events_.capacity() < size) {
    this->active_events_.reserve(size);
  } else {
    // activeなクライアントが1000以上減ったら容量をリサイズする
    if (this->active_events_.capacity() - size > 1000)
      std::vector<struct pollfd>(size).swap(this->active_events_);
  }
}

/**
 * @brief 発生したeventがread eventかを判定
 * pipeの書き込み口がcloseされた場合は、POLLINまたはPOLLHUPが設定される（環境依存）
 * POLLHUPはクライアントソケットがcloseされた際に発生するイベントで、read
 * eventとして処理する（readの返り値が0になる）
 *
 * @param event
 * @return true
 * @return false
 */
bool PollActiveEventManager::isReadEvent(const void *event) {
  const pollfd *poll_fd = static_cast<const pollfd *>(event);
  return (poll_fd->revents & POLLIN) && !isErrorEvent(event) && !isEofEvent(event);
}

bool PollActiveEventManager::isWriteEvent(const void *event) {
  const pollfd *poll_fd = static_cast<const pollfd *>(event);
  return poll_fd->revents & POLLOUT && !isErrorEvent(event) && !isEofEvent(event);
}

/**
 * @brief 発生したeventがerror eventかを判定
 * POLLERR: error発生
 * POLLNVAL: fdがopenされていない
 *
 * @param event
 * @return true
 * @return false
 */
bool PollActiveEventManager::isErrorEvent(const void *event) {
  const pollfd *poll_fd = static_cast<const pollfd *>(event);
  return (poll_fd->revents & POLLERR) || (poll_fd->revents & POLLNVAL);
}

bool PollActiveEventManager::isEofEvent(const void *event) {
  const pollfd *poll_fd = static_cast<const pollfd *>(event);
#if defined(__linux__)
  return (poll_fd->revents & POLLRDHUP);
#else
  // linux以外はEOF検知できない.
  (void)poll_fd;
  return false;
#endif
}
