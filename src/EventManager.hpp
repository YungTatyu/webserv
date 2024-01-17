#ifndef EVENT_MANAGER_HPP
# define EVENT_MANAGER_HPP

# include <poll.h>
# include <vector>

class EventManager
{
	public:
		std::vector<struct pollfd> fds;
		void updateEvents( int fd, short events );
		// void setEvents();
};

#endif
