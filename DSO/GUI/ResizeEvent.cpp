#include <GUI/ResizeEvent.h>

ResizeEvent::ResizeEvent(const SkISize &previousSize, const SkISize &currentSize) : Event(Type::Resize), m_previousSize(previousSize), m_currentSize(currentSize) {

}

ResizeEvent::~ResizeEvent() {

}

