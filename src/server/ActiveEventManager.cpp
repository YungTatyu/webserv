#include "ActiveEventManager.hpp"

void ActiveEventManager::addEvent( const struct pollfd pfd )
{
	this->active_events_.push_back( pfd );
}

void ActiveEventManager::clearAllEvents()
{
	this->active_events_.clear();
}

const std::vector<struct pollfd>& ActiveEventManager::getAllPollfd() const
{
	return this->active_events_;
}

struct pollfd ActiveEventManager::getPollfd( const int fd ) const
{
	for ( std::vector<struct pollfd>::const_iterator cur = active_events_.begin(); cur != active_events_.end(); ++cur )
	{
	       if ( cur->fd == fd )
	       {
		       return *cur;
	       }
	}

	// if no matching fd is found, return invalid fd.
	return ( genPollFd( -1, 0, 0 ) );
}

struct pollfd ActiveEventManager::genPollFd( const int fd, const short events, const short revents )
{
	struct pollfd tmp;
	tmp.fd = fd;
	tmp.events = events;
	tmp.revents = revents;
	return tmp;
}

bool ActiveEventManager::isInvalidFd( const struct pollfd& pfd )
{
	return pfd.fd == -1;
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
