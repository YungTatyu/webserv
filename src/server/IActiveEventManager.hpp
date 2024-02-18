#ifndef IACTIVE_EVENT_MANAGER_HPP
#define IACTIVE_EVENT_MANAGER_HPP

class IActiveEventManager
{
	public:
		virtual ~IActiveEventManager() {}
		virtual const void	*getActiveEvents() = 0;
		virtual void	addEvent(const void *event) = 0;
		virtual void	clearAllEvents() = 0;
		virtual bool	isReadEvent(const void *event) = 0;
		virtual bool	isWriteEvent(const void *event) = 0;
};

#endif
