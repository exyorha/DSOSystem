#ifndef GUI__LAYOUT_ITEM__H
#define GUI__LAYOUT_ITEM__H

#include <GUI/Object.h>
#include <SkSize.h>
#include <SkRect.h>

enum class ViewVisibility : uint32_t {
    Gone,
    Hidden,
    Visible
};

class LayoutItem : public Object {
public:
    LayoutItem();
    virtual ~LayoutItem();

    inline int32_t x() const { return m_geometry.x(); }
    inline int32_t y() const { return m_geometry.y(); }
    inline SkIPoint position() const {
        return SkIPoint::Make(m_geometry.x(), m_geometry.y());
    }

    inline void move(const SkIPoint &pos) { setGeometry(SkIRect::MakeXYWH(pos.x(), pos.y(), m_geometry.width(), m_geometry.height())); }
    inline void move(int32_t x, int32_t y) { setGeometry(SkIRect::MakeXYWH(x, y, m_geometry.width(), m_geometry.height())); }

    inline int32_t width() const { return m_geometry.width(); }
    inline int32_t height() const { return m_geometry.height(); }

    inline SkISize size() const { return m_geometry.size(); }
    inline void resize(const SkISize &size) {
        setGeometry(SkIRect::MakeXYWH(m_geometry.x(), m_geometry.y(), size.width(), size.height()));
    }
    inline void resize(int32_t width, int32_t height) {
        setGeometry(SkIRect::MakeXYWH(m_geometry.x(), m_geometry.y(), width, height));
    }
    
    inline const SkIRect &geometry() const { return m_geometry; }
    inline void setGeometry(const SkIRect &geometry) {
        setGeometryImpl(geometry);
    }
    inline void setGeometry(const SkIPoint &pos, const SkISize &size) {
        setGeometry(SkIRect::MakeXYWH(pos.x(), pos.y(), size.width(), size.height()));
    }
    inline void setGeometry(int32_t x, int32_t y, int32_t w, int32_t h) {
        setGeometry(SkIRect::MakeXYWH(x, y, w, h));
    }

    inline ViewVisibility visibility() const { return m_visibility; }
    inline void setVisibility(ViewVisibility visibility) {
        setVisibilityImpl(visibility);
    }

    inline bool isVisible() const { return m_visibility == ViewVisibility::Visible; }

    inline SkISize minimumSize() const { return m_minimumSize; }
    inline void setMinimumSize(const SkISize &size) { m_minimumSize = size; }
    inline void setMinimumSize(int32_t width, int32_t height) { setMinimumSize(SkISize::Make(width, height)); }
    inline int32_t minimumWidth() const { return m_minimumSize.width(); }
    inline void setMinimumWidth(int32_t width) { m_minimumSize.set(width, m_minimumSize.height()); }
    inline int32_t minimumHeight() const { return m_minimumSize.height(); }
    inline void setMinimumHeight(int32_t height) { m_minimumSize.set(height, m_minimumSize.height()); }

    inline SkISize maximumSize() const { return m_maximumSize; }
    inline void setMaximumSize(const SkISize &size) { m_maximumSize = size; }
    inline void setMaximumSize(int32_t width, int32_t height) { setMaximumSize(SkISize::Make(width, height)); }
    inline int32_t maximumWidth() const { return m_maximumSize.width(); }
    inline void setMaximumWidth(int32_t width) { m_maximumSize.set(width, m_maximumSize.height()); }
    inline int32_t maximumHeight() const { return m_maximumSize.height(); }
    inline void setMaximumHeight(int32_t height) { m_maximumSize.set(height, m_maximumSize.height()); }

    virtual SkISize minimumSizeHint() const;
    virtual SkISize maximumSizeHint() const;

    SkISize effectiveMinimumSize() const;
    SkISize effectiveMaximumSize() const;

protected:
    virtual void setGeometryImpl(const SkIRect &geometry);
    virtual void setVisibilityImpl(ViewVisibility visibility);

private:
    SkIRect m_geometry;
    ViewVisibility m_visibility;
    SkISize m_minimumSize, m_maximumSize;
};

#endif
