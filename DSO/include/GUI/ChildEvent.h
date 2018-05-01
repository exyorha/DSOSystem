#ifndef GUI__CHILD_EVENT__H
#define GUI__CHILD_EVENT__H

#include <GUI/Event.h>

class Object;

class ChildEvent final : public Event {
public:
    ChildEvent(Type type, Object *child);
    virtual ~ChildEvent();

    inline Object *child() const { return m_child; }

private:
    Object *m_child;
};

#endif
