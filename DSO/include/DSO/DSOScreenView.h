#ifndef DSO__DSO_SCREEN_VIEW__H
#define DSO__DSO_SCREEN_VIEW__H

#include <GUI/View.h>
#include <SkBitmap.h>

#include <vector>

class DSOAcquisition;

class DSOScreenView final : public View {
public:
    DSOScreenView();
    virtual ~DSOScreenView();

    void initializeAcquisition(DSOAcquisition &acquisition);

protected:
    virtual void paintEvent(PaintEvent *event) override;
    virtual void resizeEvent(ResizeEvent *event) override;

private:
    void initializeAccumulators(const SkISize &size);

    static void accumulatorReleaseProc(void *memory, void *context);

    struct Accumulator {
        SkBitmap bitmap;
    };

    std::vector<Accumulator> m_accumulators;

    //SkBitmap m_accumulator;
    DSOAcquisition *m_acquisition;
};

#endif
