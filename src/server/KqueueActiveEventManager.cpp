#include "KqueueActiveEventManager.hpp"
#if defined(KQUEUE_AVAILABLE)

KqueueActiveEventManager::KqueueActiveEventManager() {}

KqueueActiveEventManager::KqueueActiveEventManager(const KqueueActiveEventManager &other) { *this = other; }

KqueueActiveEventManager::~KqueueActiveEventManager() {}

KqueueActiveEventManager &KqueueActiveEventManager::operator=(const KqueueActiveEventManager &other) {
  if (this != &other) {
    this->active_events_ = other.active_events_;
    this->active_events_num_ = other.active_events_num_;
  }
  return *this;
}

void *KqueueActiveEventManager::getActiveEvents() { return static_cast<void *>(&(this->active_events_)); }

int KqueueActiveEventManager::getActiveEventsNum() { return this->active_events_num_; }

void KqueueActiveEventManager::setActiveEventsNum(int num) { this->active_events_num_ = num; }

/**
 * @brief Kqueueでは使用しない
 * kevent()が発生したイベントを設定してくれるため
 *
 */
void KqueueActiveEventManager::addEvent(const void *event) { static_cast<void>(event); }

void KqueueActiveEventManager::clearAllEvents() {
  this->active_events_.clear();
  // vectorのメモリを毎回解放するべきか、それとも確保した領域を引き続き使うべきか？
  // イベントが大量に発生した場合、メモリをたくさん使うので解放することが重要だと思う
  // 次に発生するイベントの数が同様に多いとは限らない
  // ただイベントが発生する度に新しい領域を確保すると、メモリ確保・解放のオーバーヘッドが発生する
  // this->active_events_.shrink_to_fit();
}

/**
 * @brief 多くのクライアントが接続し、active_events_が確保したメモリを解放するためのメソッド
 */
void KqueueActiveEventManager::reallocActiveEvents(std::size_t size) {
  // Kqueueは容量だけでなく、要素も確保しないといけない
  if (this->active_events_.size() < size) {
    if (this->active_events_.capacity() < size) this->active_events_.reserve(size);
    this->active_events_.resize(size);
    return;
  }
  // 1000以上のクライアントの接続が切れたら容量をリサイズする
  if (this->active_events_.size() - size > 1000) std::vector<struct kevent>(size).swap(this->active_events_);
}
}

/**
 * @brief ソケットが閉じられた、もしくはerror発生した場合はreadイベントとして対応しない
 * 不必要なreadを避けるため
 *
 * @param event
 * @return true
 * @return false
 */
bool KqueueActiveEventManager::isReadEvent(const void *event) {
  const struct kevent *kq_e = static_cast<const struct kevent *>(event);
  return kq_e->filter == EVFILT_READ && !isErrorEvent(event) && !isEofEvent(event);
}

/**
 * @brief ソケットが閉じられた、もしくはerror発生した場合はwriteイベントとして対応しない
 *
 * @param event
 * @return true
 * @return false
 */
bool KqueueActiveEventManager::isWriteEvent(const void *event) {
  const struct kevent *kq_e = static_cast<const struct kevent *>(event);
  return kq_e->filter == EVFILT_WRITE && !isErrorEvent(event) && !isEofEvent(event);
}

/**
 * @brief エラーが発生した
 *
 * @param event
 * @return true
 * @return false
 */
bool KqueueActiveEventManager::isErrorEvent(const void *event) {
  const struct kevent *kq_e = static_cast<const struct kevent *>(event);
  return kq_e->flags & EV_ERROR;
}

/**
 * @brief eofを感知した(ソケットが閉じられた)
 *
 * @param event
 * @return true
 * @return false
 */
bool KqueueActiveEventManager::isEofEvent(const void *event) {
  const struct kevent *kq_e = static_cast<const struct kevent *>(event);
  return (kq_e->flags & EV_EOF) && !isErrorEvent(event);
}

#endif
