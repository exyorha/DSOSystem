#include <GUI/Event.h>

Event::Event(Event::Type type) : m_type(type), m_accepted(true) {

}

Event::~Event() {

}

