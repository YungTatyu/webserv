#ifndef POLL_ACTIVE_EVENT_MANAGER_HPP
#define POLL_ACTIVE_EVENT_MANAGER_HPP

#include <vector>

#include "IActiveEventManager.hpp"

class PollActiveEventManager : public IActiveEventManager
{
	public:
		PollActiveEventManager();
		~PollActiveEventManager();
		void	addEvent(const void *event);
		const void	*getActiveEvents();
		void	clearAllEvents();
		bool	isReadEvent(const void *event);
		bool	isWriteEvent(const void *event);
	private:
		std::vector<struct pollfd> active_events_;
};

#endif
