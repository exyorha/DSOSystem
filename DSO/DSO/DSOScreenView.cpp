#include <DSO/DSOScreenView.h>
#include <DSO/DSOAcquisition.h>
#include <DSO/DSOAccumulator.h>
#include <GUI/PaintEvent.h>
#include <GUI/ResizeEvent.h>

#include <SkCanvas.h>
#include <SkPixelRef.h>
#include <SkShader.h>

DSOScreenView::DSOScreenView() : m_acquisition(nullptr) {

}

DSOScreenView::~DSOScreenView() {

}

void DSOScreenView::initializeAcquisition(DSOAcquisition &acquisition) {
    m_acquisition = &acquisition;

    m_accumulators.resize(m_acquisition->channelCount());
    initializeAccumulators(size());
}

void DSOScreenView::paintEvent(PaintEvent *event) {
    auto canvas = event->canvas();
    canvas->clear(SK_ColorBLACK);
    
    int xDivisions = 10;
    int yDivisions = 8;
    int subdivisions = 5;

    float markLength = 5;

    SkPaint graticulePaint;
    graticulePaint.setStyle(SkPaint::kFill_Style);
    graticulePaint.setColor(SK_ColorGRAY);

    for (int xCell = 0; xCell < xDivisions; xCell++) {
        for (int subdivision = 0; subdivision < subdivisions; subdivision++) {
            if (xCell == 0 && subdivision == 0)
                continue;

            float xPos = (xCell + subdivision * 1.0f / subdivisions) * (width() - 1) / xDivisions;

            canvas->drawLine(xPos, 0, xPos, markLength, graticulePaint);
            canvas->drawLine(xPos, height(), xPos, height() - markLength, graticulePaint);

            canvas->drawLine(xPos, height() / 2.0f - markLength / 2.0f, xPos, height() / 2.0f + markLength / 2.0f, graticulePaint);
        }

        if (xCell != 0) {
            for (int yCell = 0; yCell <= yDivisions; yCell++) {
                for (int subdivision = 0; subdivision <= subdivisions; subdivision++) {
                    float xPos = xCell * (width() - 1) / xDivisions;
                    float yPos = (yCell + subdivision * 1.0f / subdivisions) * (height() - 1) / yDivisions;

                    canvas->drawPoint(xPos, yPos, graticulePaint);
                }
            }
        }
    }

    for (int yCell = 0; yCell < yDivisions; yCell++) {
        for (int subdivision = 0; subdivision < subdivisions; subdivision++) {
            if (yCell == 0 && subdivision == 0)
                continue;

            float yPos = (yCell + subdivision * 1.0f / subdivisions) * (height() - 1) / yDivisions;

            canvas->drawLine(0, yPos, markLength, yPos, graticulePaint);
            canvas->drawLine(width(), yPos, width() - markLength, yPos, graticulePaint);

            canvas->drawLine(width() / 2.0f - markLength / 2.0f, yPos, width() / 2.0f + markLength / 2.0f, yPos, graticulePaint);
        }

        if (yCell != 0) {
            for (int xCell = 0; xCell <= xDivisions; xCell++) {
                for (int subdivision = 0; subdivision <= subdivisions; subdivision++) {
                    float yPos = yCell * (height() - 1) / yDivisions;
                    float xPos = (xCell + subdivision * 1.0f / subdivisions) * (width() - 1) / xDivisions;

                    canvas->drawPoint(xPos, yPos, graticulePaint);
                }
            }
        }
    }

    for (size_t index = 0, count = m_accumulators.size(); index < count; index++) {
        auto &acc = m_accumulators[index];

        SkPaint channelPaint;
        channelPaint.setShader(SkShader::MakeColorShader(SK_ColorGREEN));
        canvas->drawBitmap(acc.bitmap, 0.0f, 0.0f, &channelPaint);
    }
}

void DSOScreenView::resizeEvent(ResizeEvent *event) {
    initializeAccumulators(event->currentSize());
}

void DSOScreenView::initializeAccumulators(const SkISize &size) {
    if (!m_acquisition || size.isEmpty())
        return;

    for (size_t index = 0, count = m_accumulators.size(); index < count; index++) {
        auto &acc = m_accumulators[index];
        
        auto accumulator = m_acquisition->allocateChannelAccumulator(index, size.width(), size.height());
        
        SkImageInfo imageInfo = SkImageInfo::Make(size.width(), size.height(), kAlpha_8_SkColorType, kPremul_SkAlphaType);

        auto bareAccumulator = accumulator.release();
        acc.bitmap.installPixels(imageInfo, const_cast<void *>(bareAccumulator->base()), bareAccumulator->pitch(), accumulatorReleaseProc, bareAccumulator);
    }
}

void DSOScreenView::accumulatorReleaseProc(void *memory, void *context) {
    (void)memory;
    delete static_cast<DSOAccumulator *>(context);
}
