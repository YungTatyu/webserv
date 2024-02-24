#ifndef SELECT_ACTIVE_EVENT_MANAGER_HPP
#define SELECT_ACTIVE_EVENT_MANAGER_HPP

#include <vector>

#include "IActiveEventManager.hpp"

#include <sys/select.h>

struct SelectEvent
{
	enum SELECT_EVENT
	{
		SELECT_READ,
		SELECT_WRITE,
	};
	int	fd_;
	SELECT_EVENT	event_;
	SelectEvent(const int fd, const SELECT_EVENT event) : fd_(fd), event_(event) {}
};

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
		std::vector<SelectEvent> active_events_;
		int active_events_num_;
};

#endif
