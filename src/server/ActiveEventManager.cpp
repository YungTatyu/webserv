#include "ActiveEventManager.hpp"

void ActiveEventManager::addEvent( const struct pollfd pfd )
{
	this->active_events_.push_back( pfd );
}

void ActiveEventManager::clearAllEvents()
{
	this->active_events_.clear();
}

const std::vector<struct pollfd>& ActiveEventManager::getActiveEvents() const
{
	return this->active_events_;
}

/**
 * @brief 発生したeventがread eventかを判定
 * pipeの書き込み口がcloseされた場合は、POLLINまたはPOLLHUPが設定される（環境依存）
 * POLLHUPはクライアントソケットがcloseされた際に、発生するイベントで、read eventとして処理する（readの返り値が0になる）
 * 
 * @param pfd 
 * @return true 
 * @return false 
 */
bool ActiveEventManager::isReadEvent( const struct pollfd& pfd )
{
	return (pfd.revents & POLLIN) || (pfd.revents & POLLHUP);
}

bool ActiveEventManager::isWriteEvent( const struct pollfd& pfd )
{
	return pfd.revents & POLLOUT;
}

/**
 * @brief 発生したeventがerror eventかを判定
 * POLLERR: error発生
 * POLLNVAL: fdがopenされていない
 * 
 * @param pfd 
 * @return true 
 * @return false 
 */
bool ActiveEventManager::isErrorEvent( const struct pollfd& pfd )
{
	return (pfd.revents & POLLERR) || (pfd.revents & POLLNVAL);
}
