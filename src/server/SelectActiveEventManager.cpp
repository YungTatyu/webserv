#include "SelectActiveEventManager.hpp"

SelectActiveEventManager::SelectActiveEventManager() {}

SelectActiveEventManager::~SelectActiveEventManager() {}

void	*SelectActiveEventManager::getActiveEvents()
{
	return static_cast<void*>(&(this->active_events_));
}

int	SelectActiveEventManager::getActiveEventsNum()
{
	return this->active_events_.size();
}

void	SelectActiveEventManager::setActiveEventsNum(const int num)
{
	static_cast<void>(num);
	this->active_events_num_ = this->active_events_.size();
}

void	SelectActiveEventManager::addEvent(const void *event)
{
	const SelectEvent	*select_event = static_cast<const SelectEvent*>(event);
	this->active_events_.push_back(*select_event);
}

void	SelectActiveEventManager::clearAllEvents()
{
	this->active_events_.clear();
	// TODO: メモリ解放するか否か
	// this->active_events_.resize(0);
}

/**
 * selectは引数が多いため、使用しない
*/
bool	SelectActiveEventManager::isReadEvent(const void *event)
{
	static_cast<void>(event);
	return false;
}

/**
 * selectは引数が多いため、使用しない
*/
bool	SelectActiveEventManager::isWriteEvent(const void *event)
{
	static_cast<void>(event);
	return false;
}

/**
 * selectは引数が多いため、使用しない
*/

bool	SelectActiveEventManager::isErrorEvent(const void *event)
{
	static_cast<void>(event);
	return false;
}
