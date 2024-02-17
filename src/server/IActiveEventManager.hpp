#ifndef IACTIVE_EVENT_MANAGER_HPP
#define IACTIVE_EVENT_MANAGER_HPP

class IActiveEventManager
{
	public:
		virtual ~IActiveEventManager() {}
		virtual void	addEvent(const void *evnet) = 0;
		virtual void	clearAllEvents() = 0;
		virtual void	isReadEvent(const void *event) = 0;
		virtual void	isWriteEvent(const void *event) = 0;
};

#endif
