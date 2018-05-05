#include <GUI/KeyEvent.h>

KeyEvent::KeyEvent(Type type, uint32_t keyCode) : Event(type), m_keyCode(keyCode) {

}

KeyEvent::~KeyEvent() {

}
