#include "KqueueActiveEventManager.hpp"
#if defined(KQUEUE_AVAILABLE)

KqueueActiveEventManager::KqueueActiveEventManager() {}

KqueueActiveEventManager::~KqueueActiveEventManager() {}

void	*KqueueActiveEventManager::getActiveEvents()
{
	return static_cast<void*>(&(this->active_events_));
}

int	KqueueActiveEventManager::getActiveEventsNum()
{
	return this->active_events_num_;
}

void	KqueueActiveEventManager::setActiveEventsNum(const int num)
{
	this->active_events_num_ = num;
}

/**
 * @brief Kqueueでは使用しない
 * kevent()が発生したイベントを設定してくれるため
 * 
 */
void	KqueueActiveEventManager::addEvent(const void *event)
{
	static_cast<void>(event);
}

void	KqueueActiveEventManager::clearAllEvents()
{
	this->active_events_.clear();
	// vectorのメモリを毎回解放するべきか、それとも確保した領域を引き続き使うべきか？
	// イベントが大量に発生した場合、メモリをたくさん使うので解放することが重要だと思う
	// 次に発生するイベントの数が同様に多いとは限らない
	// ただイベントが発生する度に新しい領域を確保すると、メモリ確保・解放のオーバーヘッドが発生する
	// this->active_events_.shrink_to_fit();
}

/**
 * @brief ソケットが閉じられた、もしくはerror発生した場合はreadイベントとして対応しない
 * 不必要なreadを避けるため
 * 
 * @param event 
 * @return true 
 * @return false 
 */
bool	KqueueActiveEventManager::isReadEvent(const void *event)
{
	const struct kevent	*kq_e = static_cast<const struct kevent*>(event);
	return kq_e->filter == EVFILT_READ && !isErrorEvent(event);
}

/**
 * @brief ソケットが閉じられた、もしくはerror発生した場合はwriteイベントとして対応しない
 * 
 * @param event 
 * @return true 
 * @return false 
 */
bool	KqueueActiveEventManager::isWriteEvent(const void *event)
{
	const struct kevent	*kq_e = static_cast<const struct kevent*>(event);
	return kq_e->filter == EVFILT_WRITE && !isErrorEvent(event);
}

/**
 * @brief ソケットが閉じられたもしくはエラーが発生した
 * 
 * @param event 
 * @return true 
 * @return false 
 */
bool	KqueueActiveEventManager::isErrorEvent(const void *event)
{
	const struct kevent	*kq_e = static_cast<const struct kevent*>(event);
	return (kq_e->flags & EV_EOF) || (kq_e->flags & EV_ERROR);
}

#endif
