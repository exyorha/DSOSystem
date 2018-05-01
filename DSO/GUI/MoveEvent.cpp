#include <GUI/MoveEvent.h>

MoveEvent::MoveEvent(const SkIPoint &previousPosition, const SkIPoint &position) : Event(Type::Move), m_previousPosition(previousPosition), m_position(position) {

}

MoveEvent::~MoveEvent() {

}
