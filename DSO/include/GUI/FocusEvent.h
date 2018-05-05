#ifndef GUI__FOCUS_EVENT__H
#define GUI__FOCUS_EVENT__H

#include <GUI/Event.h>

class FocusEvent final : public Event {
public:
	FocusEvent(Type type);
	~FocusEvent();

	inline bool gotFocus() const {
		return type() == Type::FocusIn;
	}

	inline bool lostFocus() const {
		return type() == Type::FocusOut;
	}
};

#endif
