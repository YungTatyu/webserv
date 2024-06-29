#include "SelectActiveEventManager.hpp"

SelectActiveEventManager::SelectActiveEventManager() {}

SelectActiveEventManager::SelectActiveEventManager(const SelectActiveEventManager &other) { *this = other; }

SelectActiveEventManager::~SelectActiveEventManager() {}

SelectActiveEventManager &SelectActiveEventManager::operator=(const SelectActiveEventManager &other) {
  if (this != &other) {
    this->active_events_ = other.active_events_;
    this->active_events_num_ = other.active_events_num_;
  }
  return *this;
}

void *SelectActiveEventManager::getActiveEvents() { return static_cast<void *>(&(this->active_events_)); }

int SelectActiveEventManager::getActiveEventsNum() { return this->active_events_.size(); }

void SelectActiveEventManager::setActiveEventsNum(int num) {
  static_cast<void>(num);
  this->active_events_num_ = this->active_events_.size();
}

void SelectActiveEventManager::addEvent(const void *event) {
  const SelectEvent *select_event = static_cast<const SelectEvent *>(event);
  this->active_events_.push_back(*select_event);
}

void SelectActiveEventManager::clearAllEvents() {
  this->active_events_.clear();
  // TODO: メモリ解放するか否か
  // this->active_events_.resize(0);
}

bool SelectActiveEventManager::isReadEvent(const void *event) {
  const SelectEvent *sel_event = static_cast<const SelectEvent *>(event);
  return sel_event->event_ == SelectEvent::SELECT_READ;
}

bool SelectActiveEventManager::isWriteEvent(const void *event) {
  const SelectEvent *sel_event = static_cast<const SelectEvent *>(event);
  return sel_event->event_ == SelectEvent::SELECT_WRITE;
}

/**
 * selectはerrorを判定できない
 */
bool SelectActiveEventManager::isErrorEvent(const void *event) {
  static_cast<void>(event);
  return false;
}

bool SelectActiveEventManager::isEofEvent(const void *event) {
  static_cast<void>(event);
  return false;
}
