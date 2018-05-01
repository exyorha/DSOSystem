#ifndef GUI__RESIZE_EVENT__H
#define GUI__RESIZE_EVENT__H

#include <GUI/Event.h>
#include <SkSize.h>

class ResizeEvent final : public Event {
public:
    ResizeEvent(const SkISize &previousSize, const SkISize &currentSize);
    virtual ~ResizeEvent();

    inline const SkISize &previousSize() const { return m_previousSize; }
    inline const SkISize &currentSize() const { return m_currentSize; }

private:
    SkISize m_previousSize;
    SkISize m_currentSize;
};

#endif
