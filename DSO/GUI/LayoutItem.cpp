#include <GUI/LayoutItem.h>


LayoutItem::LayoutItem() : m_geometry(SkIRect::MakeEmpty()), m_visibility(ViewVisibility::Gone),
	m_minimumSize(SkISize::Make(0, 0)),
	m_maximumSize(SkISize::Make(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max())) {

}

LayoutItem::~LayoutItem() {

}

void LayoutItem::setGeometryImpl(const SkIRect &geometry) {
	m_geometry = geometry;
}

void LayoutItem::setVisibilityImpl(ViewVisibility visibility) {
	m_visibility = visibility;
}

SkISize LayoutItem::minimumSizeHint() const {
	return SkISize::Make(0, 0);
}

SkISize LayoutItem::maximumSizeHint() const {
	return SkISize::Make(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
}

SkISize LayoutItem::effectiveMinimumSize() const {
	auto hint = minimumSizeHint();

	return SkISize::Make(
		std::max(m_minimumSize.width(), hint.width()),
		std::max(m_minimumSize.height(), hint.height())
	);
}

SkISize LayoutItem::effectiveMaximumSize() const {
	auto hint = maximumSizeHint();

	return SkISize::Make(
		std::min(m_maximumSize.width(), hint.width()),
		std::min(m_maximumSize.height(), hint.height())
	);
}

