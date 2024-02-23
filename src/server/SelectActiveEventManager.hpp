#ifndef SELECT_ACTIVE_EVENT_MANAGER_HPP
#define SELECT_ACTIVE_EVENT_MANAGER_HPP

#include <vector>

#include "IActiveEventManager.hpp"

#include <sys/select.h>

class SelectActiveEventManager : public IActiveEventManager
{
	public:
		SelectActiveEventManager();
		~SelectActiveEventManager();
		void	*getActiveEvents();
		int	getActiveEventsNum();
		void	setActiveEventsNum(const int num);
		void	addEvent(const void *event);
		void	clearAllEvents();
		bool	isReadEvent(const void *event);
		bool	isWriteEvent(const void *event);
		bool	isErrorEvent(const void *event);
	private:
		std::vector<struct fd_set> active_events_;
		int active_events_num_;
};

#endif
