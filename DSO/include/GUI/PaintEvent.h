#ifndef GUI__PAINT_EVENT__H
#define GUI__PAINT_EVENT__H

#include <GUI/Event.h>

class SkCanvas;

class PaintEvent final : public Event {
public:
    explicit PaintEvent(SkCanvas *canvas);
    virtual ~PaintEvent();

    inline SkCanvas *canvas() const { return m_canvas; }

private:
    SkCanvas *m_canvas;
};

#endif
