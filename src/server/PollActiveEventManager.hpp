#ifndef POLL_ACTIVE_EVENT_MANAGER_HPP
#define POLL_ACTIVE_EVENT_MANAGER_HPP

#include <vector>

#include "IActiveEventManager.hpp"

class PollActiveEventManager : public IActiveEventManager
{
	public:
		PollActiveEventManager();
		~PollActiveEventManager();
		void	*getActiveEvents();
		int	getActiveEventsNum();
		void	setActiveEventsNum(const int num);
		void	addEvent(const void *event);
		void	clearAllEvents();
		bool	isReadEvent(const void *event);
		bool	isWriteEvent(const void *event);
		bool	isErrorEvent(const void *event);
	private:
		std::vector<struct pollfd> active_events_;
		int active_events_num_;
};

#endif
