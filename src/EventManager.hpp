#ifndef EVENT_MANAGER_HPP
# define EVENT_MANAGER_HPP

# include <poll.h>
# include <vector>

class EventManager
{
	public:
		std::vector<struct pollfd> fds;
		void updateEvents( int fd, short events );
		void addPollfd( struct pollfd pfd );
		std::vector<struct pollfd> getAllPollfd();
		struct pollfd getPollfd( int fd );

		static struct pollfd genPollFd( int fd, short events, short revents );
		static bool isFdNegative(const struct pollfd& pfd);
};

#endif
