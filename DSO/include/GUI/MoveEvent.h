#ifndef GUI__MOVE_EVENT__H
#define GUI__MOVE_EVENT__H

#include <GUI/Event.h>
#include <SkPoint.h>

class MoveEvent final : public Event {
public:
    MoveEvent(const SkIPoint &previousPosition, const SkIPoint &position);
    virtual ~MoveEvent();

    inline const SkIPoint &previousPosition() const { return m_previousPosition; }
    inline const SkIPoint &position() const { return m_position; }

private:
    SkIPoint m_previousPosition;
    SkIPoint m_position;
};

#endif
