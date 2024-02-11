#ifndef ACTIVE_EVENT_MANAGER_HPP
# define ACTIVE_EVENT_MANAGER_HPP

# include <poll.h>
# include <vector>

/**
 * @brief 発生したイベントを管理する
 * 
 */
class ActiveEventManager
{
	public:
		std::vector<struct pollfd> active_events_;
		void addEvent( const struct pollfd pfd );
		const std::vector<struct pollfd>& getAllPollfd() const;
		struct pollfd getPollfd( const int fd ) const;
		void clearAllEvents();

		static struct pollfd genPollFd( const int fd, const short events, const short revents );
		static bool isInvalidFd( const struct pollfd& pfd );
		static bool isReadEvent( const struct pollfd& pfd );
		static bool isWriteEvent( const struct pollfd& pfd );
		static bool isErrorEvent( const struct pollfd& pfd );
};

#endif
