#ifndef ACTIVE_EVENT_MANAGER_HPP
# define ACTIVE_EVENT_MANAGER_HPP

# include <poll.h>
# include <vector>

class ActiveEventManager
{
	public:
		std::vector<struct pollfd> active_events_;
		void updateEvents( const int fd, const short events );
		void addEvent( const struct pollfd pfd );
		const std::vector<struct pollfd>& getAllPollfd() const;
		struct pollfd getPollfd( const int fd ) const;
		void clearAllEvents();

		static struct pollfd genPollFd( const int fd, const short events, const short revents );
		static bool isInvalidFd( const struct pollfd& pfd );
};

#endif
