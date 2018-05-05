#ifndef GUI__VIEW__H
#define GUI__VIEW__H

#include <GUI/LayoutItem.h>
#include <GUI/LayoutConstraint.h>
#include <Logging/LogFacility.h>

#include <SkRefCnt.h>
#include <SkPoint.h>
#include <SkSize.h>
#include <SkRegion.h>

class SkSurface;

class MoveEvent;
class ResizeEvent;
class PaintEvent;

class View : public LayoutItem {
public:
    View();
    virtual ~View();
    
    void drawView(const sk_sp<SkSurface> &surface, const SkIPoint &translation, const SkRegion &visibleRegion);
    
    inline void update() {
        update(SkIRect::MakeXYWH(0, 0, width(), height()));
    }

    inline void update(int32_t x, int32_t y, int32_t w, int32_t h) {
        update(SkIRect::MakeXYWH(x, y, w, h));
    }

    inline void update(const SkIRect &rect) {
        update(SkRegion(rect));
    }

    void update(SkRegion &&region);
    
    virtual bool event(Event *event) override;

    bool isVisibleOnScreen() const;

    inline bool isLayoutEnabled() const { return m_layoutEnabled; }
    void setLayoutEnabled(bool layoutEnabled);

    void updateLayout();

    virtual SkISize minimumSizeHint() const override;
    virtual SkISize maximumSizeHint() const override;

protected:
    virtual void setGeometryImpl(const SkIRect &geometry) override;
    virtual void setVisibilityImpl(ViewVisibility visibility) override;

    virtual void moveEvent(MoveEvent *event);
    virtual void resizeEvent(ResizeEvent *event);
    virtual void showEvent(Event *event);
    virtual void hideEvent(Event *event);
    virtual void paintEvent(PaintEvent *event);

    LayoutConstraint *createConstraint(
        float leftMultiplier, LayoutItem *leftItem, LayoutAttribute leftAttribute,
        LayoutConstraint::Relation relation,
        float rightMultiplier, LayoutItem *rightItem, LayoutAttribute rightAttribute, float constant, float penalty = 0.0f);

private:
    void pushGeometryUpdates(const SkIRect &current, const SkIRect &previous);

    enum class LayoutSolveMode {
        Update,
        MinimumSize,
        MaximumSize
    };

    SkISize solveLayout(LayoutSolveMode mode);

    SkIRect m_lastVisibleGeometry;
    bool m_visibleOnScreen;
    bool m_layoutEnabled;
};

extern LogFacility viewLog;

#endif
