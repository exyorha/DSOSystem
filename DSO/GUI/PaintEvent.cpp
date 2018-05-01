#include <GUI/PaintEvent.h>

PaintEvent::PaintEvent(SkCanvas *canvas) : Event(Type::Paint), m_canvas(canvas) {

}

PaintEvent::~PaintEvent() {

}

