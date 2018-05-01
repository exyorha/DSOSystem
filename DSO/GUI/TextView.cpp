#include <GUI/TextView.h>
#include <GUI/PaintEvent.h>

#include <SkCanvas.h>

TextView::TextView() {

}

TextView::~TextView() {

}

void TextView::setText(const std::string &text) {
    m_text = text;
    update();
}

void TextView::paintEvent(PaintEvent *event) {
    View::paintEvent(event);
    
    auto canvas = event->canvas();
    SkPaint paint;
    
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);

    SkPaint::FontMetrics metrics;
    paint.getFontMetrics(&metrics);
    canvas->drawText(m_text.data(), m_text.size(), 0, -metrics.fAscent - metrics.fDescent, paint);
}

SkISize TextView::minimumSizeHint() const {
    int minHeight = sizeForWidth(std::numeric_limits<int32_t>::max()).height();
    int minWidth = sizeForWidth(0).width();

    return SkISize::Make(minWidth, minHeight);
}

SkISize TextView::sizeForWidth(int32_t width) const {
    SkPaint paint;

    //paint.setTextSize(64.0f);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);
    
    SkRect bounds;
    paint.measureText(m_text.data(), m_text.size(), &bounds);
    return SkISize::Make(static_cast<int32_t>(ceilf(bounds.width())) + 1, static_cast<int32_t>(ceilf(bounds.height())) + 1);
}

