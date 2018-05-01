#include <GUI/ChildEvent.h>

ChildEvent::ChildEvent(Type type, Object *child) : Event(type), m_child(child) {

}

ChildEvent::~ChildEvent() {

}

