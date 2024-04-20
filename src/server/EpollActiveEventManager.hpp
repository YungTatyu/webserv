#ifndef EPOLL_ACTIVE_EVENT_MANAGER_HPP
#define EPOLL_ACTIVE_EVENT_MANAGER_HPP

# include "conf.hpp"

#if defined(EPOLL_AVAILABLE)

# include "IActiveEventManager.hpp"
# include <vector>

class EpollActiveEventManager : public IActiveEventManager
{
	public:
		EpollActiveEventManager();
		~EpollActiveEventManager();
		void	*getActiveEvents();
		int	getActiveEventsNum();
		void	setActiveEventsNum(const int num);
		void	addEvent(const void *event);
		void	clearAllEvents();
		bool	isReadEvent(const void *event);
		bool	isWriteEvent(const void *event);
		bool	isErrorEvent(const void *event);
	private:
		std::vector<struct epoll_event> active_events_;
		int active_events_num_;
};

#endif
#endif
