#ifndef GUI__KEY_EVENT__H
#define GUI__KEY_EVENT__H

#include <GUI/Event.h>

class KeyEvent final : public Event {
public:
	KeyEvent(Type type, uint32_t keyCode);
	~KeyEvent();

	inline uint32_t keyCode() const { return m_keyCode; }

private:
	uint32_t m_keyCode;
};

#endif
