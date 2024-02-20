#include "PollActiveEventManager.hpp"
#include <poll.h>

PollActiveEventManager::PollActiveEventManager() {}

PollActiveEventManager::~PollActiveEventManager() {}

void	PollActiveEventManager::addEvent(const void *event)
{
	const pollfd	*poll_fd = static_cast<const pollfd*>(event);
	this->active_events_.push_back(*poll_fd);
}

void	*PollActiveEventManager::getActiveEvents()
{
	return static_cast<void*>(&(this->active_events_));
}

int	PollActiveEventManager::getActiveEventsNum()
{
	return this->active_events_.size();
}

void	PollActiveEventManager::setActiveEventsNum(const int num)
{
	this->active_events_num_ = num;
}

void	PollActiveEventManager::clearAllEvents()
{
	this->active_events_.clear();
}

/**
 * @brief 発生したeventがread eventかを判定
 * pipeの書き込み口がcloseされた場合は、POLLINまたはPOLLHUPが設定される（環境依存）
 * POLLHUPはクライアントソケットがcloseされた際に発生するイベントで、read eventとして処理する（readの返り値が0になる）
 * 
 * @param event 
 * @return true 
 * @return false 
 */
bool	PollActiveEventManager::isReadEvent(const void *event)
{
	const pollfd	*poll_fd = static_cast<const pollfd*>(event);
	return (poll_fd->revents & POLLIN) || (poll_fd->revents & POLLHUP);
}

bool	PollActiveEventManager::isWriteEvent(const void *event)
{
	const pollfd	*poll_fd = static_cast<const pollfd*>(event);
	return poll_fd->revents & POLLOUT;
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
bool	PollActiveEventManager::isErrorEvent(const void *event)
{
	const pollfd	*poll_fd = static_cast<const pollfd*>(event);
	return (poll_fd->revents & POLLERR) || (poll_fd->revents & POLLNVAL);
}
